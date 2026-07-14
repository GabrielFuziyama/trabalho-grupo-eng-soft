

#include <QtTest/QtTest>
#include <QtDebug>
#include <QMessageBox>
#include <QTimer>

#include "../QuartaWidgetApp/loginDialogBox.h"
#include "ui_logindialogbox.h"

class TestGUI: public QObject
{
    Q_OBJECT


private slots:
    void execDialog(); // para usar esse SKIP os outros
    void loginWindow();
    void timeOut();

private:
    LoginDialog d;
};


void TestGUI::execDialog(){

     //QSKIP("TEST QDialog.exec()");

    // TESTE PARA CAIXAS DE DIALOGO DISPARADAS COM EXEC() ao inves de SHOW()
    // execute and check result
    int ret = d.exec();
    //QCOMPARE(ret, static_cast<int>(QDialog::Rejected));
    QCOMPARE(ret, static_cast<int>(QDialog::Accepted));
}

void TestGUI::loginWindow()
{
        QSKIP("TEST QDialog.show()");

        // necessario para matar o MessageBox ao final do test, sem isso, o teste termina sua execução
        // antes do message box aparacer, tentei milhões de outras maneiras
        QTimer::singleShot(500, this, SLOT(timeOut()));

        // NÂO PRECISA
        //d.show();

        // verifiva de o widget foi criado
        QVERIFY2(d.ui->nomeTextEdit, "campo NOME nao foi criado "); // QVERIFY2 permite imprimir uma mensagem caso o erro ocorra
        QVERIFY2(d.ui->senhaTextEdit, "campo SENHA nao foi criado ");

        // Preenche os campos com login e senhas corretos
        QTest::keyClicks(d.ui->nomeTextEdit, "tiago");
        QTest::keyClicks(d.ui->senhaTextEdit, "carneiro");

        // Verifica se os nomes foram corretamente preenchidos (pode-se considerar redundante)
        QVERIFY(d.ui->nomeTextEdit->toPlainText()== QString("tiago"));
        QCOMPARE(d.ui->senhaTextEdit->toPlainText(), QString("carneiro"));

        // pressiona o botão direito do mouse on pushButton "continue"
        QTest::mouseClick(d.ui->continueButton, Qt::LeftButton);

        // ou essa linha ou a de cima, as duas juntas não!!!
        // pressiona o botão direito do mouse on pushButton "sair"
        //QTest::mouseClick(d.ui->sairButton, Qt::LeftButton);


        // TENTATIVAS FRUSTRADAS DE ENCONTRAR a caixa de mensagem
        qDebug() <<"1. ACTIVE POPUP WIDGET!!!! " << QApplication::activePopupWidget();
        qDebug() <<"2. ACTIVE WINDOW!!!!       " << QApplication::activeWindow();
        qDebug() <<"3. ACTIVE MODAL WIDGETS!!!!" << QApplication::activeModalWidget();
        qDebug() <<"4. TOP LEVEL WIDGETS!!!!   " << QApplication::topLevelWidgets();
        qDebug() <<"5. TOP LEVEL WINDOS!!!!    " << QApplication::topLevelWindows();
        qDebug() <<"6. FOCUS OBJECT!!!!        " << QApplication::focusObject();
        qDebug() <<"7. FOCUS WINDOW!!!!        " << QApplication::focusWindow();
        qDebug() <<"8. FOCUS WIDGET!!!!        " << QApplication::focusWidget();
        qDebug() <<"9. APPLICATION CHILDREN!!!!" << qApp->children();
        qDebug() <<"10. DIALOG CHILDREN!!!!     " << d.children();

        // TENTATIVAS FRUSTRADAS DE FECHAR a caixa de mensagem
        //QTest::keyClick(QApplication::activePopupWidget(), Qt::Key_Tab);
        //QTest::keyClick(QApplication::activePopupWidget(), Qt::Key_Space);
        //post an Escape key press and release event
        //QApplication::sendEvent(&d, new QKeyEvent(QEvent::KeyPress  , Qt::Key_Escape, Qt::NoModifier) );
        //QApplication::notify(&d, new QKeyEvent(QEvent::KeyPress  , Qt::Key_Escape, Qt::NoModifier) );
        //QApplication::sendEvent(QApplication::activeWindow(), new QKeyEvent(QEvent::KeyPress  , Qt::Key_Escape, Qt::NoModifier) );
}


void TestGUI::timeOut(){

    QSKIP("SKIP SEM FECHAR MESSAGE_BOX");

    QWidgetList allToplevelWidgets = QApplication::topLevelWidgets();
    foreach (QWidget *w, allToplevelWidgets) {
        qDebug() << "ENTREI:" << w;
        if (w->inherits("QMessageBox")) {
          QMessageBox *mb = qobject_cast<QMessageBox *>(w);
          qDebug() << "*******************************************************.....";
          qDebug() << "ACHEI....." << mb;
           QTest::keyClick(mb, Qt::Key_Escape);
        }
    }
}


QTEST_MAIN(TestGUI)
#include "unitTest.moc"
