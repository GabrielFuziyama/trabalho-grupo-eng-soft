#include <QtTest/QtTest>
#include <QMessageBox>
#include <QTimer>
#include <QPushButton>

#include "main_window.h"
#include "database.h"

class TestGUI: public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    
    void testCadastrarNovaOrdemDeServicoComSucesso_data();
    void testCadastrarNovaOrdemDeServicoComSucesso();
    
    void testConsultarOrdensDeServicoPorNomeDoCliente_data();
    void testConsultarOrdensDeServicoPorNomeDoCliente();

    void timeOutMsgBox();

private:
    MarmoManagerApp* app;
    QString expectedMsgBoxText;
    bool msgBoxFound;
};

void TestGUI::initTestCase() {
    // Setup test database
    Database::getInstance().setDbName("test_marmomanager_gui.db");
    Database::getInstance().initDb();
    
    app = new MarmoManagerApp();
}

void TestGUI::cleanupTestCase() {
    delete app;
    QFile::remove("test_marmomanager_gui.db");
}

void TestGUI::testCadastrarNovaOrdemDeServicoComSucesso_data() {
    QTest::addColumn<QString>("cliente");
    QTest::addColumn<QString>("contato");
    QTest::addColumn<QString>("medidas");
    QTest::addColumn<QString>("material");
    
    QTest::newRow("Cadastro Simples") << "Cliente Teste GUI" << "9999-9999" << "2x2" << "Marmore Branco";
}

void TestGUI::testCadastrarNovaOrdemDeServicoComSucesso() {
    QFETCH(QString, cliente);
    QFETCH(QString, contato);
    QFETCH(QString, medidas);
    QFETCH(QString, material);

    // Certificar que a aba de cadastro está ativa (índice 0)
    app->m_tabs->setCurrentIndex(0);

    // Preencher formulário
    QTest::keyClicks(app->m_cadCliente, cliente);
    QTest::keyClicks(app->m_cadContato, contato);
    QTest::keyClicks(app->m_cadMedidas, medidas);
    QTest::keyClicks(app->m_cadMaterial, material);

    // Configurar mensagem esperada no QMessageBox e agendar o fechamento
    expectedMsgBoxText = "Ordem de Serviço #1 cadastrada com sucesso!"; // Supondo que seja o ID 1
    msgBoxFound = false;
    QTimer::singleShot(200, this, SLOT(timeOutMsgBox()));

    // Encontrar o botão de salvar e clicar
    // O botão foi adicionado ao layout, mas não tem ponteiro na classe. Podemos encontrá-mo.
    QPushButton* btnSalvar = app->m_tabCadastro->findChild<QPushButton*>();
    if (!btnSalvar) {
        // Como o botão não tem nome, vamos procurar por texto
        QList<QPushButton*> buttons = app->m_tabCadastro->findChildren<QPushButton*>();
        for(auto* btn : buttons) {
            if(btn->text() == "Salvar OS") {
                btnSalvar = btn;
                break;
            }
        }
    }
    
    QVERIFY2(btnSalvar != nullptr, "Botão 'Salvar OS' não encontrado");
    
    QTest::mouseClick(btnSalvar, Qt::LeftButton);

    QVERIFY2(msgBoxFound, "Caixa de mensagem de sucesso não apareceu ou não tinha o texto esperado.");
    
    // Verificar se o formulário foi limpo
    QCOMPARE(app->m_cadCliente->text(), QString(""));
}

void TestGUI::testConsultarOrdensDeServicoPorNomeDoCliente_data() {
    QTest::addColumn<QString>("buscaCliente");
    QTest::addColumn<int>("expectedRowCount");

    QTest::newRow("Busca Vazia (Todos)") << "" << 1;
    QTest::newRow("Busca Cliente Existente") << "Cliente Teste GUI" << 1;
    QTest::newRow("Busca Cliente Inexistente") << "Fantasma" << 0;
}

void TestGUI::testConsultarOrdensDeServicoPorNomeDoCliente() {
    QFETCH(QString, buscaCliente);
    QFETCH(int, expectedRowCount);

    // Mudar para a aba de consulta (índice 1)
    app->m_tabs->setCurrentIndex(1);

    // Desmarcar filtro de data se estiver marcado
    app->m_searchUseDate->setChecked(false);

    // Preencher a busca
    app->m_searchInput->clear();
    QTest::keyClicks(app->m_searchInput, buscaCliente);

    // Encontrar botão de buscar
    QPushButton* btnBuscar = nullptr;
    QList<QPushButton*> buttons = app->m_tabConsulta->findChildren<QPushButton*>();
    for(auto* btn : buttons) {
        if(btn->text() == "Buscar") {
            btnBuscar = btn;
            break;
        }
    }
    
    QVERIFY2(btnBuscar != nullptr, "Botão 'Buscar' não encontrado");
    
    QTest::mouseClick(btnBuscar, Qt::LeftButton);

    // Checar se a tabela populou a quantidade certa
    QCOMPARE(app->m_osTable->rowCount(), expectedRowCount);
}

void TestGUI::timeOutMsgBox() {
    QWidgetList allToplevelWidgets = QApplication::topLevelWidgets();
    foreach (QWidget *w, allToplevelWidgets) {
        if (w->inherits("QMessageBox")) {
            QMessageBox *mb = qobject_cast<QMessageBox *>(w);
            if (mb->text() == expectedMsgBoxText) {
                msgBoxFound = true;
            }
            QTest::keyClick(mb, Qt::Key_Escape);
        }
    }
}

QTEST_MAIN(TestGUI)
#include "test_func_gui.moc"
