#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H

/**
 * @file auth_controller.h
 * @brief Regras de negócio de autenticação, autorização e usuários.
 */

#include "user_model.h"

#include <memory>
#include <QVector>

class UserRepository;

/**
 * @class AuthController
 * @brief Serviço da camada de negócios para controle de acesso.
 *
 * Valida dados, aplica as permissões dos perfis master e padrão e delega a
 * persistência ao componente de dados sem expor repositórios à apresentação.
 */
class AuthController {
public:
    /** Constrói o controlador e sua dependência privada de persistência. */
    AuthController();
    /** Libera a implementação de persistência encapsulada. */
    ~AuthController();

    AuthController(const AuthController&) = delete;
    AuthController& operator=(const AuthController&) = delete;

    /**
     * @brief Autentica credenciais informadas na tela de login.
     * @param username Identificador do usuário, sem distinção entre maiúsculas e minúsculas.
     * @param password Senha em texto temporário, nunca persistida diretamente.
     * @return Resultado contendo estado, usuário autenticado e eventual mensagem.
     */
    AuthenticationResult authenticate(const QString& username, const QString& password) const;

    /**
     * @brief Cadastra um usuário de perfil padrão.
     * @param actor Usuário master que autoriza a operação.
     * @param nome Nome de exibição do novo usuário.
     * @param username Identificador único desejado.
     * @param password Senha inicial.
     * @param passwordConfirmation Confirmação da senha inicial.
     * @return Identificador relacional do usuário criado.
     * @throws std::invalid_argument Quando a autorização ou os dados forem inválidos.
     */
    int createStandardUser(const UserData& actor, const QString& nome,
                           const QString& username, const QString& password,
                           const QString& passwordConfirmation);

    /**
     * @brief Ativa ou desativa um usuário padrão.
     * @param actor Usuário master que autoriza a operação.
     * @param userId Identificador do usuário afetado.
     * @param active Novo estado de acesso.
     */
    void setUserActive(const UserData& actor, int userId, bool active);

    /**
     * @brief Redefine a senha de um usuário padrão.
     * @param actor Usuário master que autoriza a operação.
     * @param userId Identificador do usuário afetado.
     * @param newPassword Nova senha.
     * @param confirmation Confirmação da nova senha.
     */
    void resetUserPassword(const UserData& actor, int userId,
                           const QString& newPassword, const QString& confirmation);

    /**
     * @brief Altera a senha do próprio usuário autenticado.
     * @param user Sessão que solicita a alteração.
     * @param currentPassword Senha atual para reautenticação.
     * @param newPassword Nova senha.
     * @param confirmation Confirmação da nova senha.
     */
    void changeOwnPassword(const UserData& user, const QString& currentPassword,
                           const QString& newPassword, const QString& confirmation);

    /**
     * @brief Lista usuários para a área administrativa.
     * @param actor Usuário master que solicita a listagem.
     * @return Usuários cadastrados, ordenados por perfil e nome.
     */
    QVector<UserData> listUsers(const UserData& actor) const;

    /** @return Verdadeiro enquanto a credencial inicial do master ainda estiver ativa. */
    bool isUsingInitialMasterPassword() const;

private:
    static void requireMaster(const UserData& actor);
    static void validatePassword(const QString& password, const QString& confirmation);

    std::unique_ptr<UserRepository> m_repository;
};

#endif // AUTH_CONTROLLER_H
