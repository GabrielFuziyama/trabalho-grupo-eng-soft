#include <QtTest/QtTest>

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

#include "auth_controller.h"
#include "database.h"
#include "login_dialog.h"

class TestLoginGUI : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testEstadoInicialDaTela();
    void testRejeitaDadosInvalidos_data();
    void testRejeitaDadosInvalidos();
    void testMostrarEOcultarSenha();
    void testLoginMasterPeloTeclado();
    void testLoginUsuarioPadrao();
    void testBloqueiaUsuarioDesativado();
};

void TestLoginGUI::initTestCase() {
    QFile::remove("test_marmomanager_login.db");
    Database::getInstance().setDbName("test_marmomanager_login.db");
    Database::getInstance().initDb();
}

void TestLoginGUI::cleanupTestCase() {
    QFile::remove("test_marmomanager_login.db");
}

void TestLoginGUI::testEstadoInicialDaTela() {
    LoginDialog login;
    auto* username = login.findChild<QLineEdit*>("loginUsername");
    auto* password = login.findChild<QLineEdit*>("loginPassword");
    auto* button = login.findChild<QPushButton*>("loginButton");
    auto* error = login.findChild<QLabel*>("loginError");
    auto* showPassword = login.findChild<QCheckBox*>("showLoginPassword");

    QVERIFY2(username, "Campo de usuário não encontrado");
    QVERIFY2(password, "Campo de senha não encontrado");
    QVERIFY2(button, "Botão Entrar não encontrado");
    QVERIFY2(error, "Área de mensagem de erro não encontrada");
    QVERIFY2(showPassword, "Controle Mostrar senha não encontrado");
    QVERIFY(username->text().isEmpty());
    QVERIFY(password->text().isEmpty());
    QCOMPARE(password->echoMode(), QLineEdit::Password);
    QVERIFY(button->isDefault());
    QVERIFY(error->text().isEmpty());
}

void TestLoginGUI::testRejeitaDadosInvalidos_data() {
    QTest::addColumn<QString>("username");
    QTest::addColumn<QString>("password");
    QTest::addColumn<QString>("expectedMessage");

    QTest::newRow("campos-vazios")
        << "" << "" << "Informe o usuário e a senha.";
    QTest::newRow("usuario-vazio")
        << "" << "Master@123" << "Informe o usuário e a senha.";
    QTest::newRow("senha-vazia")
        << "master" << "" << "Informe o usuário e a senha.";
    QTest::newRow("usuario-inexistente")
        << "nao.existe" << "Master@123" << "Usuário ou senha inválidos.";
    QTest::newRow("senha-incorreta")
        << "master" << "SenhaErrada123" << "Usuário ou senha inválidos.";
}

void TestLoginGUI::testRejeitaDadosInvalidos() {
    QFETCH(QString, username);
    QFETCH(QString, password);
    QFETCH(QString, expectedMessage);

    LoginDialog login;
    auto* usernameInput = login.findChild<QLineEdit*>("loginUsername");
    auto* passwordInput = login.findChild<QLineEdit*>("loginPassword");
    auto* button = login.findChild<QPushButton*>("loginButton");
    auto* error = login.findChild<QLabel*>("loginError");
    QVERIFY(usernameInput && passwordInput && button && error);

    usernameInput->setText(username);
    passwordInput->setText(password);
    QTest::mouseClick(button, Qt::LeftButton);

    QCOMPARE(login.result(), 0);
    QCOMPARE(error->text(), expectedMessage);
    QVERIFY(passwordInput->text().isEmpty());
    QVERIFY(button->isEnabled());
    QVERIFY(!login.authenticatedUser().isAuthenticated());
}

void TestLoginGUI::testMostrarEOcultarSenha() {
    LoginDialog login;
    auto* password = login.findChild<QLineEdit*>("loginPassword");
    auto* showPassword = login.findChild<QCheckBox*>("showLoginPassword");
    QVERIFY(password && showPassword);

    login.show();
    QTRY_VERIFY(login.isVisible());

    password->setText("SenhaVisivel123");
    QCOMPARE(password->echoMode(), QLineEdit::Password);

    showPassword->setFocus();
    QTest::keyClick(showPassword, Qt::Key_Space);
    QVERIFY(showPassword->isChecked());
    QCOMPARE(password->echoMode(), QLineEdit::Normal);
    QCOMPARE(password->text(), QString("SenhaVisivel123"));

    QTest::keyClick(showPassword, Qt::Key_Space);
    QVERIFY(!showPassword->isChecked());
    QCOMPARE(password->echoMode(), QLineEdit::Password);
}

void TestLoginGUI::testLoginMasterPeloTeclado() {
    LoginDialog login;
    auto* username = login.findChild<QLineEdit*>("loginUsername");
    auto* password = login.findChild<QLineEdit*>("loginPassword");
    QVERIFY(username && password);

    QTest::keyClicks(username, "master");
    QTest::keyClick(username, Qt::Key_Return);
    QTest::keyClicks(password, "Master@123");
    QTest::keyClick(password, Qt::Key_Return);

    QCOMPARE(login.result(), static_cast<int>(QDialog::Accepted));
    QVERIFY(login.authenticatedUser().isAuthenticated());
    QVERIFY(login.authenticatedUser().isMaster());
    QCOMPARE(login.authenticatedUser().username, QString("master"));
    QVERIFY(password->text().isEmpty());
}

void TestLoginGUI::testLoginUsuarioPadrao() {
    AuthController auth;
    const UserData master = auth.authenticate("master", "Master@123").user;
    const int id = auth.createStandardUser(
        master, "Operador de Login", "operador.login", "Operador123", "Operador123");
    QVERIFY(id > 0);

    LoginDialog login;
    auto* username = login.findChild<QLineEdit*>("loginUsername");
    auto* password = login.findChild<QLineEdit*>("loginPassword");
    auto* button = login.findChild<QPushButton*>("loginButton");
    QVERIFY(username && password && button);

    QTest::keyClicks(username, "OPERADOR.LOGIN");
    QTest::keyClicks(password, "Operador123");
    QTest::mouseClick(button, Qt::LeftButton);

    QCOMPARE(login.result(), static_cast<int>(QDialog::Accepted));
    QVERIFY(login.authenticatedUser().isAuthenticated());
    QVERIFY(!login.authenticatedUser().isMaster());
    QCOMPARE(login.authenticatedUser().username, QString("operador.login"));
    QCOMPARE(login.authenticatedUser().nome, QString("Operador de Login"));
}

void TestLoginGUI::testBloqueiaUsuarioDesativado() {
    AuthController auth;
    const UserData master = auth.authenticate("master", "Master@123").user;
    const int id = auth.createStandardUser(
        master, "Usuário Bloqueado", "usuario.bloqueado", "Bloqueado123", "Bloqueado123");
    QVERIFY(id > 0);
    auth.setUserActive(master, id, false);

    LoginDialog login;
    auto* username = login.findChild<QLineEdit*>("loginUsername");
    auto* password = login.findChild<QLineEdit*>("loginPassword");
    auto* button = login.findChild<QPushButton*>("loginButton");
    auto* error = login.findChild<QLabel*>("loginError");
    QVERIFY(username && password && button && error);

    username->setText("usuario.bloqueado");
    password->setText("Bloqueado123");
    QTest::mouseClick(button, Qt::LeftButton);

    QCOMPARE(login.result(), 0);
    QCOMPARE(error->text(), QString("Este usuário está desativado. Procure o usuário master."));
    QVERIFY(password->text().isEmpty());
    QVERIFY(!login.authenticatedUser().isAuthenticated());
}

QTEST_MAIN(TestLoginGUI)
#include "test_login_gui.moc"
