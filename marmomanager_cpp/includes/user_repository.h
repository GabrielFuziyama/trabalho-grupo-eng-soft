#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

/**
 * @file user_repository.h
 * @brief DAO relacional para autenticação e usuários.
 */

#include "user_model.h"

#include <QVector>
#include <optional>

/**
 * @class UserRepository
 * @brief Encapsula todos os comandos SQL relacionados à tabela usuarios.
 */
class UserRepository {
public:
    /**
     * @brief Consulta o usuário e verifica sua senha derivada.
     * @param username Identificador usado no login.
     * @param password Senha candidata.
     * @return Estado da tentativa e dados seguros da sessão quando autorizada.
     */
    AuthenticationResult authenticate(const QString& username, const QString& password) const;

    /**
     * @brief Insere um usuário padrão.
     * @param nome Nome de exibição.
     * @param username Identificador único normalizado.
     * @param password Senha que será derivada antes da inserção.
     * @return Chave primária do registro criado.
     */
    int createStandardUser(const QString& nome, const QString& username, const QString& password);

    /** @return Todos os usuários sem expor hashes de senha. */
    QVector<UserData> fetchAll() const;

    /**
     * @param id Chave primária procurada.
     * @return Usuário correspondente ou valor vazio.
     */
    std::optional<UserData> findById(int id) const;

    /**
     * @param id Chave primária do usuário padrão.
     * @param active Novo estado de acesso.
     * @return Verdadeiro quando exatamente um registro foi atualizado.
     */
    bool setActive(int id, bool active);

    /**
     * @param id Chave primária do usuário.
     * @param newPassword Nova senha que será derivada.
     * @return Verdadeiro quando exatamente um registro foi atualizado.
     */
    bool updatePassword(int id, const QString& newPassword);

    /**
     * @param username Identificador a consultar sem diferenciar caixa.
     * @return Verdadeiro quando o identificador já está cadastrado.
     */
    bool usernameExists(const QString& username) const;

    /** @return Verdadeiro se a senha inicial documentada do master ainda é válida. */
    bool isUsingInitialMasterPassword() const;
};

#endif // USER_REPOSITORY_H
