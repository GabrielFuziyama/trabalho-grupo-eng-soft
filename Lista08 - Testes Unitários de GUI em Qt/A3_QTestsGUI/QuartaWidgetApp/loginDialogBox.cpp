#include "loginDialogBox.h"
#include "ui_logindialogbox.h"
#include<QMessageBox>
#include<QCloseEvent>
#include<QKeyEvent>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    // try exit(), quit()
    // Há um erro de design feito no QtDesigner, a ui LoginDialog está conectada como diz a semantica abaixo
    // connect(ui->sairButton, SIGNAL(clicked()), this, SLOT(reject()));
    // para consertar a App essa conexão deve ser eliminada no QtDesigner e a aplicação inteira deve ser finalizada
    //connect(ui->sairButton, SIGNAL(clicked()), qApp, SLOT(aboutQt()));
    connect(ui->sairButton, SIGNAL(clicked()), qApp, SLOT(quit()));

    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::WindowStaysOnTopHint );
    setModal(true);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

#include<QTest>
#include<QKeyEvent>


void LoginDialog::on_continueButton_clicked(){
    QString nome = ui->nomeTextEdit->toPlainText();
    QString senha = ui->senhaTextEdit->toPlainText();

    QString resultStr;
    if(nome == "tiago" && senha == "carneiro"){
        resultStr = "BEM VINDO";
        this->accept();
    }
    else
         resultStr = "SENHA INCORRETA";

    QMessageBox::information(this, "Login", resultStr, QMessageBox::Close);
}


// para bloquear ALT+F4
void LoginDialog::closeEvent(QCloseEvent *event){
    event->ignore();
}

// para bloquear a tecla ESC
void LoginDialog::keyPressEvent(QKeyEvent *event) {
    if(event->key() != Qt::Key_Escape)
        QDialog::keyPressEvent(event);
    else { //nop
    }
}

