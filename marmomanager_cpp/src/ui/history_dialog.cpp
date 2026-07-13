#include "history_dialog.h"
#include <QVBoxLayout>
#include <QHeaderView>

StatusHistoryDialog::StatusHistoryDialog(int osId, OSController* controller, QWidget* parent)
    : QDialog(parent), m_osId(osId), m_controller(controller) {
    
    setWindowTitle(QString("Histórico de Status da OS #%1").arg(m_osId));
    resize(500, 300);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"Data", "De", "Para", "Responsável"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    layout->addWidget(m_table);
    setLayout(layout);
    
    loadHistory();
}

void StatusHistoryDialog::loadHistory() {
    QVector<HistoricoData> rows = m_controller->obterHistorico(m_osId);
    
    m_table->setRowCount(rows.size());
    for (int i = 0; i < rows.size(); ++i) {
        const auto& h = rows[i];
        m_table->setItem(i, 0, new QTableWidgetItem(h.dataAlteracao));
        m_table->setItem(i, 1, new QTableWidgetItem(h.statusAnterior));
        m_table->setItem(i, 2, new QTableWidgetItem(h.statusNovo));
        m_table->setItem(i, 3, new QTableWidgetItem(h.responsavel));
    }
}
