/**
 * @file os_controller.h
 * @brief Cabeçalho da classe OSController.
 */

#ifndef OS_CONTROLLER_H
#define OS_CONTROLLER_H

#include "os_model.h"
#include <memory>
#include <QString>
#include <stdexcept>

class OSRepository;
class HistoricoRepository;

/**
 * @class OSController
 * @brief Classe Controller na arquitetura MVC.
 * Gerencia toda a lógica de negócios da aplicação.
 */
class OSController {
public:
    /** Constrói o serviço e suas dependências privadas da camada de dados. */
    OSController();
    /** Libera os DAOs encapsulados pela implementação. */
    ~OSController();

    OSController(const OSController&) = delete;
    OSController& operator=(const OSController&) = delete;

    /**
     * @brief Cadastra uma nova OS aplicando as regras de negócio.
     * @param cliente Nome obrigatório do cliente.
     * @param contato Contato do cliente.
     * @param descricao Descrição do serviço.
     * @param medidas Medidas da peça.
     * @param material Material solicitado.
     * @param prazo Prazo no formato ISO.
     * @param status Estado inicial.
     * @param prioridade Prioridade operacional.
     * @param dificuldade Dificuldade estimada.
     * @return Identificador relacional da nova OS.
     * @throws std::invalid_argument se a validação falhar.
     */
    int cadastrarOS(const QString& cliente, const QString& contato, const QString& descricao,
                    const QString& medidas, const QString& material, const QString& prazo,
                    const QString& status, const QString& prioridade, const QString& dificuldade);

    /**
     * @brief Atualiza o status da OS e registra no histórico.
     * @param osId Identificador da OS.
     * @param statusAnterior Estado observado antes da alteração.
     * @param novoStatus Estado solicitado.
     * @param responsavel Usuário autenticado responsável.
     * @throws std::invalid_argument se a validação falhar.
     */
    void atualizarStatusOS(int osId, const QString& statusAnterior, const QString& novoStatus,
                           const QString& responsavel);

    /**
     * @brief Lista as Ordens de Serviço baseando-se em filtros.
     * @param buscaCliente Trecho opcional do cliente.
     * @param buscaStatus Estado ou Todos.
     * @param dataIni Início opcional do intervalo ISO.
     * @param dataFim Fim opcional do intervalo ISO.
     * @return Ordens que atendem aos filtros.
     */
    QVector<OSData> listarOS(const QString& buscaCliente = "", const QString& buscaStatus = "Todos",
                             const QString& dataIni = "", const QString& dataFim = "");

    /**
     * @brief Recupera o histórico de uma OS específica.
     * @param osId Identificador da ordem.
     * @return Transições em ordem cronológica inversa.
     */
    QVector<HistoricoData> obterHistorico(int osId);

private:
    std::unique_ptr<OSRepository> m_osRepo;
    std::unique_ptr<HistoricoRepository> m_histRepo;
};

#endif // OS_CONTROLLER_H
