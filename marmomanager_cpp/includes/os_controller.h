/**
 * @file os_controller.h
 * @brief Cabeçalho da classe OSController.
 */

#ifndef OS_CONTROLLER_H
#define OS_CONTROLLER_H

#include "os_repository.h"
#include "historico_repository.h"
#include <QString>
#include <stdexcept>

/**
 * @class OSController
 * @brief Classe Controller na arquitetura MVC.
 * Gerencia toda a lógica de negócios da aplicação.
 */
class OSController {
public:
    OSController();

    /**
     * @brief Cadastra uma nova OS aplicando as regras de negócio.
     * @throws std::invalid_argument se a validação falhar.
     */
    int cadastrarOS(const QString& cliente, const QString& contato, const QString& descricao,
                    const QString& medidas, const QString& material, const QString& prazo,
                    const QString& status, const QString& prioridade, const QString& dificuldade);

    /**
     * @brief Atualiza o status da OS e registra no histórico.
     * @throws std::invalid_argument se a validação falhar.
     */
    void atualizarStatusOS(int osId, const QString& statusAnterior, const QString& novoStatus,
                           const QString& responsavel);

    /**
     * @brief Lista as Ordens de Serviço baseando-se em filtros.
     */
    QVector<OSData> listarOS(const QString& buscaCliente = "", const QString& buscaStatus = "Todos",
                             const QString& dataIni = "", const QString& dataFim = "");

    /**
     * @brief Recupera o histórico de uma OS específica.
     */
    QVector<HistoricoData> obterHistorico(int osId);

private:
    OSRepository m_osRepo;
    HistoricoRepository m_histRepo;
};

#endif // OS_CONTROLLER_H
