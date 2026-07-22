#include "password_hasher.h"

#include <QCryptographicHash>
#include <QStringList>
#include <QUuid>

namespace {
constexpr int PASSWORD_ITERATIONS = 100000;
}

QByteArray PasswordHasher::derive(const QString& password, const QByteArray& salt, int iterations) {
    QByteArray digest = password.toUtf8() + salt;
    for (int i = 0; i < iterations; ++i) {
        digest = QCryptographicHash::hash(digest + salt, QCryptographicHash::Sha256);
    }
    return digest;
}

QString PasswordHasher::hash(const QString& password) {
    const QByteArray salt = QUuid::createUuid().toRfc4122().toHex();
    const QByteArray digest = derive(password, salt, PASSWORD_ITERATIONS).toHex();
    return QString("v1$%1$%2$%3")
        .arg(PASSWORD_ITERATIONS)
        .arg(QString::fromLatin1(salt))
        .arg(QString::fromLatin1(digest));
}

bool PasswordHasher::verify(const QString& password, const QString& encodedHash) {
    const QStringList parts = encodedHash.split('$');
    if (parts.size() != 4 || parts[0] != "v1") {
        return false;
    }

    bool ok = false;
    const int iterations = parts[1].toInt(&ok);
    if (!ok || iterations <= 0 || iterations > 1000000) {
        return false;
    }

    const QByteArray salt = parts[2].toLatin1();
    const QByteArray expected = parts[3].toLatin1();
    const QByteArray actual = derive(password, salt, iterations).toHex();

    if (actual.size() != expected.size()) {
        return false;
    }

    unsigned char difference = 0;
    for (int i = 0; i < actual.size(); ++i) {
        difference |= static_cast<unsigned char>(actual[i] ^ expected[i]);
    }
    return difference == 0;
}
