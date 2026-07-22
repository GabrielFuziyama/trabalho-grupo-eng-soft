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
    /** Chave primária da ordem de serviço. */
    int id;
    /** Nome do cliente solicitante. */
    QString cliente;
    /** Telefone, e-mail ou outro contato. */
    QString contato;
    /** Descrição do serviço contratado. */
    QString descricao;
    /** Medidas informadas para produção. */
    QString medidas;
    /** Material escolhido pelo cliente. */
    QString material;
    /** Data prevista de entrega no formato ISO. */
    QString prazo;
    /** Estado atual da ordem de serviço. */
    QString status;
    /** Prioridade operacional. */
    QString prioridade;
    /** Nível de dificuldade estimado. */
    QString dificuldade;
    /** Data de abertura no formato ISO. */
    QString dataAbertura;
};

/**
 * @struct HistoricoData
 * @brief Representa um registro de Histórico de mudanças.
 */
struct HistoricoData {
    /** Chave primária do evento. */
    int id;
    /** Chave estrangeira da ordem de serviço. */
    int osId;
    /** Estado existente antes da transição. */
    QString statusAnterior;
    /** Estado definido pela transição. */
    QString statusNovo;
    /** Data e hora em que a alteração ocorreu. */
    QString dataAlteracao;
    /** Nome do usuário responsável pela alteração. */
    QString responsavel;
};

#endif // OS_MODEL_H
