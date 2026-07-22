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
    /** Constrói um DAO sem estado; a conexão é obtida do Database. */
    OSRepository();

    /**
     * @brief Cria uma nova Ordem de Serviço (OS).
     * @param cliente Nome do cliente.
     * @param contato Contato do cliente.
     * @param descricao Descrição do serviço.
     * @param medidas Medidas da peça.
     * @param material Material solicitado.
     * @param prazo Prazo no formato ISO.
     * @param status Estado inicial.
     * @param prioridade Prioridade operacional.
     * @param dificuldade Dificuldade estimada.
     * @param dataAbertura Data de abertura no formato ISO.
     * @return O ID da OS recém criada.
     */
    int create(const QString& cliente, const QString& contato, const QString& descricao,
               const QString& medidas, const QString& material, const QString& prazo,
               const QString& status, const QString& prioridade, const QString& dificuldade,
               const QString& dataAbertura);

    /**
     * @brief Atualiza o status de uma OS existente.
     * @param osId Chave primária da OS.
     * @param novoStatus Estado que será persistido.
     */
    void updateStatus(int osId, const QString& novoStatus);

    /**
     * @brief Busca todas as OS que coincidem com os critérios.
     * @param buscaCliente Trecho opcional do nome do cliente.
     * @param buscaStatus Estado desejado ou Todos.
     * @param dataIni Início opcional do intervalo de abertura.
     * @param dataFim Fim opcional do intervalo de abertura.
     * @return Registros encontrados e ordenados por prazo e prioridade.
     */
    QVector<OSData> fetchAll(const QString& buscaCliente = "", 
                             const QString& buscaStatus = "Todos",
                             const QString& dataIni = "", 
                             const QString& dataFim = "");
};

#endif // OS_REPOSITORY_H
