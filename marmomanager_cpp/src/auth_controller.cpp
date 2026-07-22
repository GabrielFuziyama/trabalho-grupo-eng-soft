#include "auth_controller.h"
#include "user_repository.h"

#include <QRegularExpression>
#include <stdexcept>

AuthController::AuthController()
    : m_repository(std::make_unique<UserRepository>()) {}

AuthController::~AuthController() = default;

AuthenticationResult AuthController::authenticate(const QString& username, const QString& password) const {
    if (username.trimmed().isEmpty() || password.isEmpty()) {
        return {AuthenticationStatus::InvalidCredentials, {}, "Informe o usuário e a senha."};
    }
    return m_repository->authenticate(username, password);
}

int AuthController::createStandardUser(const UserData& actor, const QString& nome,
                                       const QString& username, const QString& password,
                                       const QString& passwordConfirmation) {
    requireMaster(actor);
    if (nome.trimmed().size() < 3) {
        throw std::invalid_argument("Informe o nome completo do usuário.");
    }

    const QString normalizedUsername = username.trimmed().toLower();
    const QRegularExpression validUsername("^[a-z0-9._-]{3,30}$");
    if (!validUsername.match(normalizedUsername).hasMatch()) {
        throw std::invalid_argument("O usuário deve ter de 3 a 30 caracteres: letras, números, ponto, hífen ou sublinhado.");
    }
    if (m_repository->usernameExists(normalizedUsername)) {
        throw std::invalid_argument("Este nome de usuário já está em uso.");
    }

    validatePassword(password, passwordConfirmation);
    return m_repository->createStandardUser(nome, normalizedUsername, password);
}

void AuthController::setUserActive(const UserData& actor, int userId, bool active) {
    requireMaster(actor);
    const auto target = m_repository->findById(userId);
    if (!target.has_value()) {
        throw std::invalid_argument("Usuário não encontrado.");
    }
    if (target->isMaster()) {
        throw std::invalid_argument("O usuário master não pode ser desativado.");
    }
    if (!m_repository->setActive(userId, active)) {
        throw std::runtime_error("Não foi possível alterar o estado do usuário.");
    }
}

void AuthController::resetUserPassword(const UserData& actor, int userId,
                                       const QString& newPassword, const QString& confirmation) {
    requireMaster(actor);
    const auto target = m_repository->findById(userId);
    if (!target.has_value()) {
        throw std::invalid_argument("Usuário não encontrado.");
    }
    if (target->isMaster()) {
        throw std::invalid_argument("Altere a senha do master pelo menu da própria sessão.");
    }
    validatePassword(newPassword, confirmation);
    if (!m_repository->updatePassword(userId, newPassword)) {
        throw std::runtime_error("Não foi possível redefinir a senha.");
    }
}

void AuthController::changeOwnPassword(const UserData& user, const QString& currentPassword,
                                       const QString& newPassword, const QString& confirmation) {
    const auto authentication = m_repository->authenticate(user.username, currentPassword);
    if (authentication.status != AuthenticationStatus::Success || authentication.user.id != user.id) {
        throw std::invalid_argument("A senha atual está incorreta.");
    }
    validatePassword(newPassword, confirmation);
    if (currentPassword == newPassword) {
        throw std::invalid_argument("A nova senha deve ser diferente da senha atual.");
    }
    if (!m_repository->updatePassword(user.id, newPassword)) {
        throw std::runtime_error("Não foi possível alterar a senha.");
    }
}

QVector<UserData> AuthController::listUsers(const UserData& actor) const {
    requireMaster(actor);
    return m_repository->fetchAll();
}

bool AuthController::isUsingInitialMasterPassword() const {
    return m_repository->isUsingInitialMasterPassword();
}

void AuthController::requireMaster(const UserData& actor) {
    if (!actor.isAuthenticated() || !actor.isMaster()) {
        throw std::invalid_argument("Apenas o usuário master pode realizar esta operação.");
    }
}

void AuthController::validatePassword(const QString& password, const QString& confirmation) {
    if (password != confirmation) {
        throw std::invalid_argument("A confirmação da senha não confere.");
    }
    if (password.size() < 8 || !password.contains(QRegularExpression("[A-Za-z]"))
        || !password.contains(QRegularExpression("[0-9]"))) {
        throw std::invalid_argument("A senha deve ter ao menos 8 caracteres, com letras e números.");
    }
}
