#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

/**
 * @file login_dialog.h
 * @brief Tela modal de autenticação do MarmoManager.
 */

#include "auth_controller.h"

#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;

/**
 * @class LoginDialog
 * @brief Captura credenciais e apresenta o resultado seguro da autenticação.
 */
class LoginDialog : public QDialog {
    Q_OBJECT

public:
    /** @param parent Widget Qt proprietário do diálogo. */
    explicit LoginDialog(QWidget* parent = nullptr);
    /** @return Usuário autenticado após o diálogo ser aceito. */
    UserData authenticatedUser() const;

private slots:
    void attemptLogin();
    void setPasswordVisible(bool visible);

private:
    AuthController m_controller;
    UserData m_authenticatedUser;
    QLineEdit* m_usernameInput;
    QLineEdit* m_passwordInput;
    QLabel* m_errorLabel;
    QPushButton* m_loginButton;
};

#endif // LOGIN_DIALOG_H
