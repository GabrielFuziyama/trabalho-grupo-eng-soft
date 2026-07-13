#include "historico_repository.h"
#include "database.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QDebug>

HistoricoRepository::HistoricoRepository() {
}

void HistoricoRepository::create(int osId, const QString& statusAnterior, const QString& statusNovo,
                                 const QString& dataAlteracao, const QString& responsavel) {
    QSqlDatabase db = Database::getInstance().getConnection();
    QSqlQuery query(db);
    
    query.prepare("INSERT INTO historico_os (os_id, status_anterior, status_novo, data_alteracao, responsavel) "
                  "VALUES (:os_id, :status_anterior, :status_novo, :data_alteracao, :responsavel)");
    query.bindValue(":os_id", osId);
    query.bindValue(":status_anterior", statusAnterior);
    query.bindValue(":status_novo", statusNovo);
    query.bindValue(":data_alteracao", dataAlteracao);
    query.bindValue(":responsavel", responsavel);
    
    if (!query.exec()) {
        qWarning() << "Failed to insert history:" << query.lastError().text();
    }
}

QVector<HistoricoData> HistoricoRepository::getByOsId(int osId) {
    QVector<HistoricoData> results;
    QSqlDatabase db = Database::getInstance().getConnection();
    QSqlQuery query(db);
    
    query.prepare("SELECT id, os_id, status_anterior, status_novo, data_alteracao, responsavel "
                  "FROM historico_os WHERE os_id = :os_id ORDER BY id DESC");
    query.bindValue(":os_id", osId);
    
    if (query.exec()) {
        while (query.next()) {
            HistoricoData h;
            h.id = query.value("id").toInt();
            h.osId = query.value("os_id").toInt();
            h.statusAnterior = query.value("status_anterior").toString();
            h.statusNovo = query.value("status_novo").toString();
            h.dataAlteracao = query.value("data_alteracao").toString();
            h.responsavel = query.value("responsavel").toString();
            results.push_back(h);
        }
    } else {
        qWarning() << "Failed to fetch history:" << query.lastError().text();
    }
    
    return results;
}
