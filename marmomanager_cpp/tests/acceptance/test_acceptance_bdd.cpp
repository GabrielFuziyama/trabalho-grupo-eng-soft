#include <QtTest/QtTest>

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>
#include <QTextEdit>
#include <QTimer>

#include "auth_controller.h"
#include "database.h"
#include "login_dialog.h"
#include "main_window.h"
#include "os_controller.h"

class TestAcceptanceBDD : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void cenarioMasterAcessaAdministracaoDeUsuarios();
    void cenarioMasterCadastraUsuarioPadrao();
    void cenarioUsuarioDesativadoNaoConsegueEntrar();
    void cenarioOperadorCadastraEAtualizaOrdemDeServico();

private:
    static void closeNextMessageBox();
};

void TestAcceptanceBDD::initTestCase() {
    QFile::remove("test_marmomanager_acceptance.db");
    Database::getInstance().setDbName("test_marmomanager_acceptance.db");
    Database::getInstance().initDb();
}

void TestAcceptanceBDD::cleanupTestCase() {
    QFile::remove("test_marmomanager_acceptance.db");
}

void TestAcceptanceBDD::closeNextMessageBox() {
    QTimer::singleShot(100, []() {
        for (QWidget* widget : QApplication::topLevelWidgets()) {
            if (auto* messageBox = qobject_cast<QMessageBox*>(widget)) {
                messageBox->accept();
            }
        }
    });
}

void TestAcceptanceBDD::cenarioMasterAcessaAdministracaoDeUsuarios() {
    // DADO que o banco foi inicializado com o usuário master
    LoginDialog login;
    auto* username = login.findChild<QLineEdit*>("loginUsername");
    auto* password = login.findChild<QLineEdit*>("loginPassword");
    auto* enterButton = login.findChild<QPushButton*>("loginButton");
    QVERIFY(username && password && enterButton);

    // QUANDO o master informa credenciais válidas
    QTest::keyClicks(username, "master");
    QTest::keyClicks(password, "Master@123");
    QTest::mouseClick(enterButton, Qt::LeftButton);

    // ENTÃO o acesso é autorizado e a administração fica disponível
    QCOMPARE(login.result(), static_cast<int>(QDialog::Accepted));
    QVERIFY(login.authenticatedUser().isMaster());

    MarmoManagerApp mainWindow(login.authenticatedUser());
    auto* tabs = mainWindow.findChild<QTabWidget*>("mainTabs");
    auto* usersTab = mainWindow.findChild<QWidget*>("usersTab");
    QVERIFY(tabs && usersTab);
    QVERIFY(tabs->indexOf(usersTab) >= 0);
}

void TestAcceptanceBDD::cenarioMasterCadastraUsuarioPadrao() {
    // DADO que o master está autenticado
    AuthController auth;
    const UserData master = auth.authenticate("master", "Master@123").user;
    QVERIFY(master.isMaster());
    MarmoManagerApp mainWindow(master);
    mainWindow.show();

    auto* name = mainWindow.findChild<QLineEdit*>("userFullName");
    auto* username = mainWindow.findChild<QLineEdit*>("newUsername");
    auto* password = mainWindow.findChild<QLineEdit*>("newUserPassword");
    auto* confirmation = mainWindow.findChild<QLineEdit*>("newUserPasswordConfirmation");
    auto* createButton = mainWindow.findChild<QPushButton*>("createUserButton");
    QVERIFY(name && username && password && confirmation && createButton);

    // QUANDO ele cadastra um usuário padrão válido
    QTest::keyClicks(name, "Atendente Aceitacao");
    QTest::keyClicks(username, "atendente.aceitacao");
    QTest::keyClicks(password, "Atendente123");
    QTest::keyClicks(confirmation, "Atendente123");
    closeNextMessageBox();
    QTest::mouseClick(createButton, Qt::LeftButton);

    // ENTÃO o novo usuário consegue autenticar-se
    const AuthenticationResult result = auth.authenticate("atendente.aceitacao", "Atendente123");
    QCOMPARE(result.status, AuthenticationStatus::Success);
    QVERIFY(result.user.isAuthenticated());
    QVERIFY(!result.user.isMaster());
}

void TestAcceptanceBDD::cenarioUsuarioDesativadoNaoConsegueEntrar() {
    // DADO que existe um usuário padrão desativado
    AuthController auth;
    const UserData master = auth.authenticate("master", "Master@123").user;
    const int userId = auth.createStandardUser(
        master, "Funcionário Desativado", "funcionario.desativado",
        "Desativado123", "Desativado123");
    auth.setUserActive(master, userId, false);

    LoginDialog login;
    auto* username = login.findChild<QLineEdit*>("loginUsername");
    auto* password = login.findChild<QLineEdit*>("loginPassword");
    auto* enterButton = login.findChild<QPushButton*>("loginButton");
    auto* error = login.findChild<QLabel*>("loginError");
    QVERIFY(username && password && enterButton && error);

    // QUANDO ele informa suas credenciais corretas
    username->setText("funcionario.desativado");
    password->setText("Desativado123");
    QTest::mouseClick(enterButton, Qt::LeftButton);

    // ENTÃO o acesso é recusado com uma mensagem objetiva
    QCOMPARE(login.result(), 0);
    QCOMPARE(error->text(), QString("Este usuário está desativado. Procure o usuário master."));
    QVERIFY(!login.authenticatedUser().isAuthenticated());
}

void TestAcceptanceBDD::cenarioOperadorCadastraEAtualizaOrdemDeServico() {
    // DADO que existe um operador padrão autenticado
    AuthController auth;
    const UserData master = auth.authenticate("master", "Master@123").user;
    auth.createStandardUser(master, "Operador Aceitação", "operador.aceitacao",
                            "Operador123", "Operador123");
    const UserData operador = auth.authenticate("operador.aceitacao", "Operador123").user;
    QVERIFY(operador.isAuthenticated());
    QVERIFY(!operador.isMaster());

    MarmoManagerApp mainWindow(operador);
    mainWindow.show();
    auto* tabs = mainWindow.findChild<QTabWidget*>("mainTabs");
    auto* customer = mainWindow.findChild<QLineEdit*>("serviceOrderCustomer");
    auto* contact = mainWindow.findChild<QLineEdit*>("serviceOrderContact");
    auto* description = mainWindow.findChild<QTextEdit*>("serviceOrderDescription");
    auto* measurements = mainWindow.findChild<QLineEdit*>("serviceOrderMeasurements");
    auto* material = mainWindow.findChild<QLineEdit*>("serviceOrderMaterial");
    auto* saveButton = mainWindow.findChild<QPushButton*>("saveServiceOrderButton");
    QVERIFY(tabs && customer && contact && description && measurements && material && saveButton);

    // QUANDO ele cadastra uma ordem de serviço para um cliente
    QTest::keyClicks(customer, "Cliente BDD");
    QTest::keyClicks(contact, "31999990000");
    QTest::keyClicks(description, "Bancada de cozinha");
    QTest::keyClicks(measurements, "2,00 x 0,60 m");
    QTest::keyClicks(material, "Granito Preto");
    closeNextMessageBox();
    QTest::mouseClick(saveButton, Qt::LeftButton);

    OSController serviceOrders;
    const QVector<OSData> storedOrders = serviceOrders.listarOS("Cliente BDD");
    QCOMPARE(storedOrders.size(), 1);
    QCOMPARE(storedOrders.first().status, QString("Aberta"));

    // E altera o status da ordem para Concluída
    tabs->setCurrentIndex(1);
    auto* table = mainWindow.findChild<QTableWidget*>("serviceOrdersTable");
    auto* newStatus = mainWindow.findChild<QComboBox*>("serviceOrderNewStatus");
    auto* responsible = mainWindow.findChild<QLineEdit*>("serviceOrderResponsible");
    auto* updateButton = mainWindow.findChild<QPushButton*>("updateServiceOrderStatusButton");
    QVERIFY(table && newStatus && responsible && updateButton);
    QCOMPARE(responsible->text(), operador.nome);
    QVERIFY(responsible->isReadOnly());
    QCOMPARE(table->rowCount(), 1);
    table->selectRow(0);
    newStatus->setCurrentText("Concluída");
    closeNextMessageBox();
    QTest::mouseClick(updateButton, Qt::LeftButton);

    // ENTÃO o SQLite preserva a OS e o histórico identifica o operador
    const QVector<OSData> updatedOrders = serviceOrders.listarOS("Cliente BDD");
    QCOMPARE(updatedOrders.size(), 1);
    QCOMPARE(updatedOrders.first().status, QString("Concluída"));
    const QVector<HistoricoData> history = serviceOrders.obterHistorico(updatedOrders.first().id);
    QCOMPARE(history.size(), 2);
    QCOMPARE(history.first().statusNovo, QString("Concluída"));
    QCOMPARE(history.first().responsavel, operador.nome);
}

QTEST_MAIN(TestAcceptanceBDD)
#include "test_acceptance_bdd.moc"
