#include "os_controller.h"
#include "historico_repository.h"
#include "os_repository.h"
#include <QDateTime>

OSController::OSController()
    : m_osRepo(std::make_unique<OSRepository>()),
      m_histRepo(std::make_unique<HistoricoRepository>()) {}

OSController::~OSController() = default;

int OSController::cadastrarOS(const QString& cliente, const QString& contato, const QString& descricao,
                              const QString& medidas, const QString& material, const QString& prazo,
                              const QString& status, const QString& prioridade, const QString& dificuldade) {
    if (cliente.trimmed().isEmpty()) {
        throw std::invalid_argument("O nome do cliente é obrigatório.");
    }
    
    QString dataAbertura = QDate::currentDate().toString(Qt::ISODate);
    
    int osId = m_osRepo->create(cliente.trimmed(), contato, descricao, medidas, material, prazo, status, prioridade, dificuldade, dataAbertura);
    if (osId <= 0) {
        throw std::runtime_error("Falha ao cadastrar a Ordem de Serviço.");
    }
    
    QString dataAlt = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    m_histRepo->create(osId, "N/A", status, dataAlt, "Sistema");
    
    return osId;
}

void OSController::atualizarStatusOS(int osId, const QString& statusAnterior, const QString& novoStatus,
                                     const QString& responsavel) {
    if (responsavel.trimmed().isEmpty()) {
        throw std::invalid_argument("Informe o nome do responsável pela alteração.");
    }
    if (statusAnterior == novoStatus) {
        throw std::invalid_argument("O status selecionado já é o status atual da OS.");
    }
    
    m_osRepo->updateStatus(osId, novoStatus);
    
    QString dataAlt = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    m_histRepo->create(osId, statusAnterior, novoStatus, dataAlt, responsavel.trimmed());
}

QVector<OSData> OSController::listarOS(const QString& buscaCliente, const QString& buscaStatus,
                                       const QString& dataIni, const QString& dataFim) {
    return m_osRepo->fetchAll(buscaCliente, buscaStatus, dataIni, dataFim);
}

QVector<HistoricoData> OSController::obterHistorico(int osId) {
    return m_histRepo->getByOsId(osId);
}
