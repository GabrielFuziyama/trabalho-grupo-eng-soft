/**
 * @file history_dialog.h
 * @brief Cabeçalho para a view StatusHistoryDialog.
 */

#ifndef HISTORY_DIALOG_H
#define HISTORY_DIALOG_H

#include <QDialog>
#include <QTableWidget>
#include "os_controller.h"

/**
 * @class StatusHistoryDialog
 * @brief Dialog (janela modal) para exibir o histórico da OS.
 */
class StatusHistoryDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Cria o diálogo e carrega o histórico da ordem indicada.
     * @param osId Identificador da ordem de serviço.
     * @param controller Serviço de negócio usado na consulta.
     * @param parent Widget Qt proprietário do diálogo.
     */
    StatusHistoryDialog(int osId, OSController* controller, QWidget* parent = nullptr);

private:
    void loadHistory();

    int m_osId;
    OSController* m_controller;
    QTableWidget* m_table;
};

#endif // HISTORY_DIALOG_H
