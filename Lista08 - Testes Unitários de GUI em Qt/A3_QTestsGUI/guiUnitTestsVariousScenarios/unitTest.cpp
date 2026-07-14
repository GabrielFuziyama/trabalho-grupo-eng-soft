#define TEST_TRANSICAO_JANELAS // ATIVE APENAS NUM SEGUNDO MOMENTO - entenda o codigo primeiro

#include <QtTest/QtTest>
#include <QtDebug>
#include <QMessageBox>
#include <QTimer>
#include <QPushButton>

#include "../QuartaWidgetApp/loginDialogBox.h"
#include "ui_logindialogbox.h"

// TESTE UNITARIO COMPORTAMENTAL( funcionalidade + transições)
class TestLoginDialogGUI: public QObject
{
    Q_OBJECT


private slots:
    void casoDeUsoPrincipal_data();
    void casoDeUsoPrincipal();


    #ifdef TEST_TRANSICAO_JANELAS
    void casoDeUsoAlternativo_data();
    void casoDeUsoAlternativo();
    #endif

    void timeOut();

private:
    LoginDialog d;
    QString msgResult;
    bool dialogoAberto;
};


void TestLoginDialogGUI::casoDeUsoPrincipal_data(){

    // ENTRADA
    QTest::addColumn<QString>("nome");
    QTest::addColumn<QString>("senha");
    QTest::addColumn<QPushButton*>("botao");
    // SAIDA
    QTest::addColumn<QString>("mensagem");
    QTest::addColumn<bool>("visivel"); // somente se quiser testar transição (parte importante do comportamento

    QTest::newRow("login correto") << "tiago" << "carneiro" << d.ui->continueButton << "BEM VINDO" << false;
    QTest::newRow("login errado")  << "Hello" << "HELLO" << d.ui->continueButton << "SENHA INCORRETA" << true;
    QTest::newRow("botão sair") << "" << "" << d.ui->sairButton << "" << false;

}

void TestLoginDialogGUI::casoDeUsoPrincipal()
{
        //QSKIP("TEST casoDeUso PRINCIPAL");

        QFETCH(QString, nome);
        QFETCH(QString, senha);
        QFETCH(QPushButton*, botao);
        QFETCH(QString, mensagem);
        QFETCH(bool, visivel);


        #ifdef TEST_TRANSICAO_JANELAS
        // ATIVAR NUM SEGUNDO MOMENTO: para fazer teste de transição
        d.show();
        #endif

        // coloca a string correta para ser processada no slot "timeOut"
        // faca isso antes de colocar o timer
        msgResult = mensagem;
        dialogoAberto = visivel;

        // necessario para matar o MessageBox ao final do test, sem isso, o teste termina sua execução
        // antes do message box aparacer, tentei milhões de outras maneiras
        QTimer::singleShot(500, this, SLOT(timeOut()));

        // Preenche os campos com login e senhas corretos
        QTest::keyClicks(d.ui->nomeTextEdit, nome);
        QTest::keyClicks(d.ui->senhaTextEdit, senha);

        // pressiona o botão direito do mouse on pushButton "continue"
        QTest::mouseClick(botao, Qt::LeftButton);
        //qDebug() << "BOTAO: " << botao  <<", "<< QDialog::Accepted <<", "<< QDialog::Rejected;
}


void TestLoginDialogGUI::timeOut(){

    //QSKIP("SKIP SEM FECHAR MESSAGE_BOX");

    qDebug() << "RESULT: " << d.result();


    #ifdef TEST_TRANSICAO_JANELAS
    // TESTE UNITARIOS NAO SÂO BONS PARA TESTAR TRANSICOES: NEM TUDO SAO FLORES
    qDebug() << "DIALOGO ABERTO? " << d.isVisible() << " == " << dialogoAberto;
    #endif

    if(msgResult != ""){

        // Verificar e fechar message box
        QWidgetList allToplevelWidgets = QApplication::topLevelWidgets();
        foreach (QWidget *w, allToplevelWidgets) {
            if (w->inherits("QMessageBox")) {
               QMessageBox *mb = qobject_cast<QMessageBox *>(w);
               //qDebug() << "*******************************************************.....";
               //qDebug() << "MessageBox: " << mb << mb->text() << "::" << msgResult;
               QCOMPARE(mb->text(), msgResult);
               QTest::keyClick(mb, Qt::Key_Escape);
            }
        }

        #ifdef TEST_TRANSICAO_JANELAS
        // verificar transições de janela
        QCOMPARE(dialogoAberto, d.isVisible());
        #endif
    }

}


#ifdef TEST_TRANSICAO_JANELAS
void TestLoginDialogGUI::casoDeUsoAlternativo_data(){
    // ENTRADA
    QTest::addColumn<QString>("evento");
    // SAIDA
    QTest::addColumn<bool>("visivel"); // somente se quiser testar transição (parte importante do comportamento

    QTest::newRow("pressinou teclas ALT + F4") << "alt+f4" << true;
    QTest::newRow("pressionou tecla ESCAPE")  << "esc" << true;
    QTest::newRow("clicou botão fechar") << "close" << true;
}


void TestLoginDialogGUI::casoDeUsoAlternativo(){

    //QSKIP("TEST casoDeUso ALTERNATIVO");

    QFETCH(QString, evento);
    QFETCH(bool, visivel);

    d.show();

    if(evento =="esc")
        QApplication::sendEvent(&d, new QKeyEvent(QEvent::KeyPress  , Qt::Key_Escape, Qt::NoModifier) );
    else
        QApplication::sendEvent(&d, new QCloseEvent());

    //qDebug() << "DIALOGO ABERTO: expected " << visivel << " == " << d.isVisible();
    QCOMPARE(visivel, d.isVisible());
}
#endif

QTEST_MAIN(TestLoginDialogGUI)
#include "unitTest.moc"
