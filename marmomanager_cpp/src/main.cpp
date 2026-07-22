#include <QApplication>
#include "application_coordinator.h"

/**
 * @brief Ponto de entrada mínimo do executável MarmoManager.
 * @param argc Quantidade de argumentos de linha de comando.
 * @param argv Argumentos repassados ao Qt.
 * @return Código de saída do loop de eventos.
 */
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");

    ApplicationCoordinator coordinator;
    coordinator.start(app);

    return app.exec();
}
