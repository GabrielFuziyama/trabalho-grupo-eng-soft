#include "user_repository.h"

#include "database.h"
#include "password_hasher.h"

#include <QDateTime>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <stdexcept>

namespace {
UserData userFromQuery(const QSqlQuery& query) {
    UserData user;
    user.id = query.value("id").toInt();
    user.nome = query.value("nome").toString();
    user.username = query.value("username").toString();
    user.perfil = query.value("perfil").toString();
    user.ativo = query.value("ativo").toBool();
    return user;
}
}

AuthenticationResult UserRepository::authenticate(const QString& username, const QString& password) const {
    QSqlQuery query(Database::getInstance().getConnection());
    query.prepare(R"(
        SELECT id, nome, username, senha_hash, perfil, ativo
        FROM usuarios
        WHERE username = :username COLLATE NOCASE
    )");
    query.bindValue(":username", username.trimmed());

    if (!query.exec() || !query.next()) {
        return {AuthenticationStatus::InvalidCredentials, {}, "Usuário ou senha inválidos."};
    }

    const UserData user = userFromQuery(query);
    if (!PasswordHasher::verify(password, query.value("senha_hash").toString())) {
        return {AuthenticationStatus::InvalidCredentials, {}, "Usuário ou senha inválidos."};
    }
    if (!user.ativo) {
        return {AuthenticationStatus::Inactive, {}, "Este usuário está desativado. Procure o usuário master."};
    }

    return {AuthenticationStatus::Success, user, {}};
}

int UserRepository::createStandardUser(const QString& nome, const QString& username, const QString& password) {
    QSqlQuery query(Database::getInstance().getConnection());
    query.prepare(R"(
        INSERT INTO usuarios (nome, username, senha_hash, perfil, ativo, criado_em)
        VALUES (:nome, :username, :senha_hash, 'padrao', 1, :criado_em)
    )");
    query.bindValue(":nome", nome.trimmed());
    query.bindValue(":username", username.trimmed().toLower());
    query.bindValue(":senha_hash", PasswordHasher::hash(password));
    query.bindValue(":criado_em", QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!query.exec()) {
        throw std::runtime_error(query.lastError().text().toStdString());
    }
    return query.lastInsertId().toInt();
}

QVector<UserData> UserRepository::fetchAll() const {
    QVector<UserData> users;
    QSqlQuery query(Database::getInstance().getConnection());
    query.exec(R"(
        SELECT id, nome, username, perfil, ativo
        FROM usuarios
        ORDER BY CASE WHEN perfil = 'master' THEN 0 ELSE 1 END, nome COLLATE NOCASE
    )");
    while (query.next()) {
        users.push_back(userFromQuery(query));
    }
    return users;
}

std::optional<UserData> UserRepository::findById(int id) const {
    QSqlQuery query(Database::getInstance().getConnection());
    query.prepare("SELECT id, nome, username, perfil, ativo FROM usuarios WHERE id = :id");
    query.bindValue(":id", id);
    if (!query.exec() || !query.next()) {
        return std::nullopt;
    }
    return userFromQuery(query);
}

bool UserRepository::setActive(int id, bool active) {
    QSqlQuery query(Database::getInstance().getConnection());
    query.prepare("UPDATE usuarios SET ativo = :ativo WHERE id = :id AND perfil = 'padrao'");
    query.bindValue(":ativo", active ? 1 : 0);
    query.bindValue(":id", id);
    return query.exec() && query.numRowsAffected() == 1;
}

bool UserRepository::updatePassword(int id, const QString& newPassword) {
    QSqlQuery query(Database::getInstance().getConnection());
    query.prepare("UPDATE usuarios SET senha_hash = :senha_hash WHERE id = :id");
    query.bindValue(":senha_hash", PasswordHasher::hash(newPassword));
    query.bindValue(":id", id);
    return query.exec() && query.numRowsAffected() == 1;
}

bool UserRepository::usernameExists(const QString& username) const {
    QSqlQuery query(Database::getInstance().getConnection());
    query.prepare("SELECT 1 FROM usuarios WHERE username = :username COLLATE NOCASE");
    query.bindValue(":username", username.trimmed());
    return query.exec() && query.next();
}

bool UserRepository::isUsingInitialMasterPassword() const {
    const auto result = authenticate("master", "Master@123");
    return result.status == AuthenticationStatus::Success && result.user.isMaster();
}
