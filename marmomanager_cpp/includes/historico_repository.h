/**
 * @file historico_repository.h
 * @brief Cabeçalho da classe HistoricoRepository.
 */

#ifndef HISTORICO_REPOSITORY_H
#define HISTORICO_REPOSITORY_H

#include "os_model.h"
#include <QVector>

/**
 * @class HistoricoRepository
 * @brief Data Access Object (DAO) para o Histórico de OS.
 */
class HistoricoRepository {
public:
    /** Constrói um DAO sem estado; a conexão é obtida do Database. */
    HistoricoRepository();

    /**
     * @brief Cria um novo registro de histórico.
     * @param osId Chave estrangeira da OS.
     * @param statusAnterior Estado anterior.
     * @param statusNovo Novo estado.
     * @param dataAlteracao Data e hora da transição.
     * @param responsavel Usuário autenticado responsável.
     */
    void create(int osId, const QString& statusAnterior, const QString& statusNovo,
                const QString& dataAlteracao, const QString& responsavel);

    /**
     * @brief Busca o histórico para uma dada OS.
     * @param osId Chave primária da OS.
     * @return Eventos do mais recente para o mais antigo.
     */
    QVector<HistoricoData> getByOsId(int osId);
};

#endif // HISTORICO_REPOSITORY_H
