#include "database.h"
#include "password_hasher.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>
#include <QDateTime>

Database& Database::getInstance() {
    static Database instance;
    return instance;
}

Database::Database() {
    QString dataDir = QDir::homePath() + "/.marmomanager";
    QDir dir;
    if (!dir.exists(dataDir)) {
        dir.mkpath(dataDir);
    }
    m_dbName = dataDir + "/marmomanager.db";
    m_db = QSqlDatabase::addDatabase("QSQLITE");
}

void Database::setDbName(const QString& dbName) {
    if (m_db.isOpen()) {
        m_db.close();
    }
    m_dbName = dbName;
}

QSqlDatabase Database::getConnection() const {
    // `database()` abre a conexão automaticamente por padrão. Como o nome do
    // arquivo precisa ser definido antes do open(), solicitamos explicitamente
    // a conexão fechada para evitar que o SQLite use um banco temporário em memória.
    QSqlDatabase db = QSqlDatabase::database(QSqlDatabase::defaultConnection, false);
    if (db.databaseName() != m_dbName) {
        if (db.isOpen()) {
            db.close();
        }
        db.setDatabaseName(m_dbName);
    }
    if (!db.isOpen()) {
        if (!db.open()) {
            qWarning() << "Failed to open database:" << db.lastError().text();
        }
    }
    if (db.isOpen()) {
        QSqlQuery pragma(db);
        if (!pragma.exec("PRAGMA foreign_keys = ON")) {
            qWarning() << "Failed to enable SQLite foreign keys:" << pragma.lastError().text();
        }
    }
    return db;
}

void Database::initDb() {
    QSqlDatabase db = getConnection();
    QSqlQuery query(db);

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS os (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            cliente TEXT NOT NULL,
            contato TEXT,
            descricao TEXT,
            medidas TEXT,
            material TEXT,
            prazo TEXT,
            status TEXT NOT NULL,
            prioridade TEXT,
            dificuldade TEXT,
            data_abertura TEXT
        )
    )");

    // Add column if not exists (ignore error if it does)
    query.exec("ALTER TABLE os ADD COLUMN data_abertura TEXT");
    query.exec("ALTER TABLE os ADD COLUMN prioridade TEXT");
    query.exec("ALTER TABLE os ADD COLUMN dificuldade TEXT");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS historico_os (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            os_id INTEGER,
            status_anterior TEXT,
            status_novo TEXT,
            data_alteracao TEXT,
            responsavel TEXT,
            FOREIGN KEY(os_id) REFERENCES os(id)
        )
    )");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS usuarios (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            nome TEXT NOT NULL,
            username TEXT NOT NULL COLLATE NOCASE UNIQUE,
            senha_hash TEXT NOT NULL,
            perfil TEXT NOT NULL CHECK(perfil IN ('master', 'padrao')),
            ativo INTEGER NOT NULL DEFAULT 1 CHECK(ativo IN (0, 1)),
            criado_em TEXT NOT NULL
        )
    )");

    query.exec(R"(
        CREATE UNIQUE INDEX IF NOT EXISTS idx_usuarios_master_unico
        ON usuarios(perfil) WHERE perfil = 'master'
    )");

    query.prepare(R"(
        INSERT INTO usuarios (nome, username, senha_hash, perfil, ativo, criado_em)
        SELECT 'Usuário Master', 'master', :senha_hash, 'master', 1, :criado_em
        WHERE NOT EXISTS (SELECT 1 FROM usuarios WHERE perfil = 'master')
    )");
    query.bindValue(":senha_hash", PasswordHasher::hash("Master@123"));
    query.bindValue(":criado_em", QDateTime::currentDateTime().toString(Qt::ISODate));
    if (!query.exec()) {
        qWarning() << "Failed to create initial master user:" << query.lastError().text();
    }
}
