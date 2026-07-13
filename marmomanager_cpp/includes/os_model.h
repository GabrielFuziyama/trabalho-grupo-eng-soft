/**
 * @file os_model.h
 * @brief Estruturas de dados para OS e Histórico.
 */

#ifndef OS_MODEL_H
#define OS_MODEL_H

#include <QString>
#include <QVector>

/**
 * @struct OSData
 * @brief Representa um registro único de Ordem de Serviço.
 */
struct OSData {
    int id;
    QString cliente;
    QString contato;
    QString descricao;
    QString medidas;
    QString material;
    QString prazo;
    QString status;
    QString prioridade;
    QString dificuldade;
    QString dataAbertura;
};

/**
 * @struct HistoricoData
 * @brief Representa um registro de Histórico de mudanças.
 */
struct HistoricoData {
    int id;
    int osId;
    QString statusAnterior;
    QString statusNovo;
    QString dataAlteracao;
    QString responsavel;
};

#endif // OS_MODEL_H
