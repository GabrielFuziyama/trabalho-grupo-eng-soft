#ifndef APPLICATION_COORDINATOR_H
#define APPLICATION_COORDINATOR_H

/**
 * @file application_coordinator.h
 * @brief Coordenação do fluxo visual de login, sessão e logout.
 */

#include "user_model.h"

#include <QObject>

class QApplication;

/**
 * @class ApplicationCoordinator
 * @brief Ponto de entrada da camada de apresentação.
 *
 * Controla a alternância entre a tela de autenticação e a janela principal,
 * sem expor detalhes das views ao executável.
 */
class ApplicationCoordinator : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Cria um coordenador associado opcionalmente a um objeto Qt pai.
     * @param parent Objeto Qt proprietário do coordenador.
     */
    explicit ApplicationCoordinator(QObject* parent = nullptr);

    /**
     * @brief Inicializa as camadas inferiores e apresenta a tela de login.
     * @param application Aplicação Qt cujo ciclo de eventos será coordenado.
     */
    void start(QApplication& application);

private:
    void showLogin();
    void showMainWindow(const UserData& user);

    QApplication* m_application = nullptr;
};

#endif // APPLICATION_COORDINATOR_H
