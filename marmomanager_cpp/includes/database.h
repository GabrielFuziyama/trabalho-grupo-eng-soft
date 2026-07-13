/**
 * @file database.h
 * @brief Arquivo de cabeçalho para o Singleton do Banco de Dados.
 */

#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QString>

/**
 * @class Database
 * @brief Classe Singleton para gerenciar a conexão com o banco de dados SQLite.
 * 
 * Garante que apenas uma instância da conexão com o banco seja criada
 * e gerencia a criação das tabelas iniciais.
 */
class Database {
public:
    /**
     * @brief Obtém a instância única do Banco de Dados.
     * @return Referência para a instância do Database.
     */
    static Database& getInstance();

    /**
     * @brief Define um nome customizado para o banco (útil para testes).
     * @param dbName Nome do arquivo SQLite.
     */
    void setDbName(const QString& dbName);

    /**
     * @brief Recupera o objeto de conexão QSqlDatabase.
     * @return A conexão QSqlDatabase.
     */
    QSqlDatabase getConnection() const;

    /**
     * @brief Inicializa o banco de dados criando as tabelas necessárias.
     */
    void initDb();

private:
    // Private constructor for Singleton
    Database();
    
    // Delete copy constructor and assignment operator
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    QString m_dbName;
    QSqlDatabase m_db;
};

#endif // DATABASE_H
