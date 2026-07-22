#ifndef PASSWORD_HASHER_H
#define PASSWORD_HASHER_H

/**
 * @file password_hasher.h
 * @brief Derivação e verificação segura das senhas persistidas.
 */

#include <QByteArray>
#include <QString>

/**
 * @class PasswordHasher
 * @brief Utilitário sem estado para hash de senha com salt aleatório.
 *
 * O valor codificado inclui versão, iterações, salt e digest. A comparação é
 * realizada em tempo constante para valores de mesmo tamanho.
 */
class PasswordHasher {
public:
    /**
     * @param password Senha original, mantida somente em memória durante o cálculo.
     * @return Representação versionada própria para persistência.
     */
    static QString hash(const QString& password);

    /**
     * @param password Senha candidata.
     * @param encodedHash Representação anteriormente produzida por hash().
     * @return Verdadeiro quando a senha corresponde ao hash armazenado.
     */
    static bool verify(const QString& password, const QString& encodedHash);

private:
    static QByteArray derive(const QString& password, const QByteArray& salt, int iterations);
};

#endif // PASSWORD_HASHER_H
