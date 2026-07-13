#include "database.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDir>

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
    m_dbName = dbName;
}

QSqlDatabase Database::getConnection() const {
    QSqlDatabase db = QSqlDatabase::database();
    db.setDatabaseName(m_dbName);
    if (!db.isOpen()) {
        if (!db.open()) {
            qWarning() << "Failed to open database:" << db.lastError().text();
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
}
