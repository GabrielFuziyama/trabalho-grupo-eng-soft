#include "login_dialog.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Acesso ao MarmoManager");
    setModal(true);
    setFixedSize(440, 410);
    setObjectName("loginDialog");

    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(44, 35, 44, 35);
    mainLayout->setSpacing(14);

    auto* title = new QLabel("MarmoManager");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 28px; font-weight: 700; color: #263238;");

    auto* subtitle = new QLabel("Entre para acessar o controle de ordens de serviço");
    subtitle->setAlignment(Qt::AlignCenter);
    subtitle->setWordWrap(true);
    subtitle->setStyleSheet("color: #607D8B; font-size: 13px;");

    auto* separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setStyleSheet("color: #CFD8DC;");

    m_usernameInput = new QLineEdit();
    m_usernameInput->setObjectName("loginUsername");
    m_usernameInput->setPlaceholderText("Nome de usuário");
    m_usernameInput->setClearButtonEnabled(true);
    m_usernameInput->setMinimumHeight(36);

    m_passwordInput = new QLineEdit();
    m_passwordInput->setObjectName("loginPassword");
    m_passwordInput->setPlaceholderText("Senha");
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_passwordInput->setMinimumHeight(36);

    auto* showPassword = new QCheckBox("Mostrar senha");
    showPassword->setObjectName("showLoginPassword");
    connect(showPassword, &QCheckBox::toggled, this, &LoginDialog::setPasswordVisible);

    m_errorLabel = new QLabel();
    m_errorLabel->setObjectName("loginError");
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->setWordWrap(true);
    m_errorLabel->setStyleSheet("color: #C62828;");
    m_errorLabel->setMinimumHeight(34);

    m_loginButton = new QPushButton("Entrar");
    m_loginButton->setObjectName("loginButton");
    m_loginButton->setDefault(true);
    m_loginButton->setMinimumHeight(40);
    m_loginButton->setCursor(Qt::PointingHandCursor);
    m_loginButton->setStyleSheet(R"(
        QPushButton { background: #2E7D32; color: white; border: 0; border-radius: 4px;
                      font-size: 14px; font-weight: 600; }
        QPushButton:hover { background: #1B5E20; }
        QPushButton:pressed { background: #164A19; }
    )");
    connect(m_loginButton, &QPushButton::clicked, this, &LoginDialog::attemptLogin);
    connect(m_passwordInput, &QLineEdit::returnPressed, this, &LoginDialog::attemptLogin);
    connect(m_usernameInput, &QLineEdit::returnPressed, m_passwordInput,
            static_cast<void (QWidget::*)()>(&QWidget::setFocus));

    mainLayout->addWidget(title);
    mainLayout->addWidget(subtitle);
    mainLayout->addWidget(separator);
    mainLayout->addWidget(new QLabel("Usuário"));
    mainLayout->addWidget(m_usernameInput);
    mainLayout->addWidget(new QLabel("Senha"));
    mainLayout->addWidget(m_passwordInput);
    mainLayout->addWidget(showPassword);
    mainLayout->addWidget(m_errorLabel);
    mainLayout->addWidget(m_loginButton);

    if (m_controller.isUsingInitialMasterPassword()) {
        auto* firstAccess = new QLabel("Primeiro acesso: usuário <b>master</b> e senha <b>Master@123</b>. "
                                       "Altere essa senha após entrar.");
        firstAccess->setObjectName("initialAccessHint");
        firstAccess->setAlignment(Qt::AlignCenter);
        firstAccess->setWordWrap(true);
        firstAccess->setStyleSheet("color: #795548; background: #FFF8E1; padding: 7px; border-radius: 3px;");
        mainLayout->addWidget(firstAccess);
        setFixedHeight(475);
    }

    m_usernameInput->setFocus();
}

UserData LoginDialog::authenticatedUser() const {
    return m_authenticatedUser;
}

void LoginDialog::attemptLogin() {
    m_errorLabel->clear();
    m_loginButton->setEnabled(false);

    const AuthenticationResult result = m_controller.authenticate(
        m_usernameInput->text(), m_passwordInput->text());

    m_loginButton->setEnabled(true);
    if (result.status == AuthenticationStatus::Success) {
        m_authenticatedUser = result.user;
        m_passwordInput->clear();
        accept();
        return;
    }

    m_errorLabel->setText(result.message);
    m_passwordInput->clear();
    m_passwordInput->setFocus();
}

void LoginDialog::setPasswordVisible(bool visible) {
    m_passwordInput->setEchoMode(visible ? QLineEdit::Normal : QLineEdit::Password);
}
