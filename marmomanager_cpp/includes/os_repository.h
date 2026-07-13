/**
 * @file os_repository.h
 * @brief Cabeçalho para a classe OSRepository.
 */

#ifndef OS_REPOSITORY_H
#define OS_REPOSITORY_H

#include "os_model.h"
#include <QVector>

/**
 * @class OSRepository
 * @brief Data Access Object (DAO) para as entidades OS.
 */
class OSRepository {
public:
    OSRepository();

    /**
     * @brief Cria uma nova Ordem de Serviço (OS).
     * @return O ID da OS recém criada.
     */
    int create(const QString& cliente, const QString& contato, const QString& descricao,
               const QString& medidas, const QString& material, const QString& prazo,
               const QString& status, const QString& prioridade, const QString& dificuldade,
               const QString& dataAbertura);

    /**
     * @brief Atualiza o status de uma OS existente.
     */
    void updateStatus(int osId, const QString& novoStatus);

    /**
     * @brief Busca todas as OS que coincidem com os critérios.
     */
    QVector<OSData> fetchAll(const QString& buscaCliente = "", 
                             const QString& buscaStatus = "Todos",
                             const QString& dataIni = "", 
                             const QString& dataFim = "");
};

#endif // OS_REPOSITORY_H
