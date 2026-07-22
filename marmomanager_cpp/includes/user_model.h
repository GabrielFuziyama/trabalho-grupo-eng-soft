#ifndef USER_MODEL_H
#define USER_MODEL_H

/**
 * @file user_model.h
 * @brief Contratos de dados compartilhados para autenticação e sessão.
 */

#include <QString>

/**
 * @struct UserData
 * @brief Dados públicos de um usuário, sem informações sensíveis.
 */
struct UserData {
    /** Chave primária no SQLite. */
    int id = 0;
    /** Nome exibido na interface e no histórico. */
    QString nome;
    /** Identificador único utilizado no login. */
    QString username;
    /** Perfil de autorização: master ou padrao. */
    QString perfil;
    /** Informa se novas autenticações estão permitidas. */
    bool ativo = false;

    /** @return Verdadeiro quando o perfil é master. */
    bool isMaster() const {
        return perfil.compare("master", Qt::CaseInsensitive) == 0;
    }

    /** @return Verdadeiro quando os dados representam uma sessão válida e ativa. */
    bool isAuthenticated() const {
        return id > 0 && ativo;
    }
};

/**
 * @enum AuthenticationStatus
 * @brief Resultado categórico de uma tentativa de login.
 */
enum class AuthenticationStatus {
    /** Credenciais válidas e usuário ativo. */
    Success,
    /** Usuário inexistente, senha incorreta ou campos incompletos. */
    InvalidCredentials,
    /** Credenciais válidas para um usuário administrativamente desativado. */
    Inactive
};

/**
 * @struct AuthenticationResult
 * @brief Resposta completa produzida pela autenticação.
 */
struct AuthenticationResult {
    /** Estado categórico da tentativa. */
    AuthenticationStatus status = AuthenticationStatus::InvalidCredentials;
    /** Dados da sessão, preenchidos apenas em caso de sucesso. */
    UserData user;
    /** Mensagem segura e adequada para apresentação ao usuário. */
    QString message;
};

#endif // USER_MODEL_H
