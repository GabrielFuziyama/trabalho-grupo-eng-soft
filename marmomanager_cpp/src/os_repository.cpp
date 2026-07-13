#include "os_repository.h"
#include "database.h"
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QDebug>

OSRepository::OSRepository() {
}

int OSRepository::create(const QString& cliente, const QString& contato, const QString& descricao,
                         const QString& medidas, const QString& material, const QString& prazo,
                         const QString& status, const QString& prioridade, const QString& dificuldade,
                         const QString& dataAbertura) {
    QSqlDatabase db = Database::getInstance().getConnection();
    QSqlQuery query(db);
    
    query.prepare("INSERT INTO os (cliente, contato, descricao, medidas, material, prazo, status, prioridade, dificuldade, data_abertura) "
                  "VALUES (:cli, :con, :desc, :med, :mat, :pra, :sta, :prio, :dif, :data)");
    query.bindValue(":cli", cliente);
    query.bindValue(":con", contato);
    query.bindValue(":desc", descricao);
    query.bindValue(":med", medidas);
    query.bindValue(":mat", material);
    query.bindValue(":pra", prazo);
    query.bindValue(":sta", status);
    query.bindValue(":prio", prioridade);
    query.bindValue(":dif", dificuldade);
    query.bindValue(":data", dataAbertura);
    
    if (!query.exec()) {
        qWarning() << "Failed to insert OS:" << query.lastError().text();
        return -1;
    }
    
    return query.lastInsertId().toInt();
}

void OSRepository::updateStatus(int osId, const QString& novoStatus) {
    QSqlDatabase db = Database::getInstance().getConnection();
    QSqlQuery query(db);
    
    query.prepare("UPDATE os SET status = :status WHERE id = :id");
    query.bindValue(":status", novoStatus);
    query.bindValue(":id", osId);
    
    if (!query.exec()) {
        qWarning() << "Failed to update OS status:" << query.lastError().text();
    }
}

QVector<OSData> OSRepository::fetchAll(const QString& buscaCliente, const QString& buscaStatus,
                                       const QString& dataIni, const QString& dataFim) {
    QVector<OSData> results;
    QSqlDatabase db = Database::getInstance().getConnection();
    QSqlQuery query(db);
    
    QString sql = "SELECT id, cliente, contato, descricao, medidas, material, prazo, status, prioridade, dificuldade, data_abertura FROM os WHERE 1=1";
    
    if (!buscaCliente.isEmpty()) {
        sql += " AND cliente LIKE :cliente";
    }
    if (buscaStatus != "Todos") {
        sql += " AND status = :status";
    }
    if (!dataIni.isEmpty() && !dataFim.isEmpty()) {
        sql += " AND data_abertura >= :dataIni AND data_abertura <= :dataFim";
    }
    
    sql += " ORDER BY prazo ASC, CASE prioridade WHEN 'Alta' THEN 1 WHEN 'Média' THEN 2 WHEN 'Baixa' THEN 3 ELSE 4 END ASC";
    
    query.prepare(sql);
    
    if (!buscaCliente.isEmpty()) {
        query.bindValue(":cliente", "%" + buscaCliente + "%");
    }
    if (buscaStatus != "Todos") {
        query.bindValue(":status", buscaStatus);
    }
    if (!dataIni.isEmpty() && !dataFim.isEmpty()) {
        query.bindValue(":dataIni", dataIni);
        query.bindValue(":dataFim", dataFim);
    }
    
    if (query.exec()) {
        while (query.next()) {
            OSData os;
            os.id = query.value("id").toInt();
            os.cliente = query.value("cliente").toString();
            os.contato = query.value("contato").toString();
            os.descricao = query.value("descricao").toString();
            os.medidas = query.value("medidas").toString();
            os.material = query.value("material").toString();
            os.prazo = query.value("prazo").toString();
            os.status = query.value("status").toString();
            os.prioridade = query.value("prioridade").toString();
            os.dificuldade = query.value("dificuldade").toString();
            os.dataAbertura = query.value("data_abertura").toString();
            results.push_back(os);
        }
    } else {
        qWarning() << "Failed to fetch OS:" << query.lastError().text();
    }
    
    return results;
}
