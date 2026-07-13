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
    HistoricoRepository();

    /**
     * @brief Cria um novo registro de histórico.
     */
    void create(int osId, const QString& statusAnterior, const QString& statusNovo,
                const QString& dataAlteracao, const QString& responsavel);

    /**
     * @brief Busca o histórico para uma dada OS.
     */
    QVector<HistoricoData> getByOsId(int osId);
};

#endif // HISTORICO_REPOSITORY_H
