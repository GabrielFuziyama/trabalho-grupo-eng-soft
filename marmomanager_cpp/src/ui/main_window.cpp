#include "main_window.h"
#include "history_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <QMessageBox>
#include <QDate>
#include <stdexcept>

MarmoManagerApp::MarmoManagerApp(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("MarmoManager - Sistema de Controle de OS");
    resize(800, 600);
    
    m_tabs = new QTabWidget(this);
    setCentralWidget(m_tabs);
    
    m_tabCadastro = new QWidget();
    m_tabConsulta = new QWidget();
    
    m_tabs->addTab(m_tabCadastro, "Cadastrar OS");
    m_tabs->addTab(m_tabConsulta, "Consultar / Atualizar OS");
    
    setupCadastroTab();
    setupConsultaTab();
    
    connect(m_tabs, &QTabWidget::currentChanged, this, &MarmoManagerApp::onTabChanged);
}

void MarmoManagerApp::setupCadastroTab() {
    QVBoxLayout* layout = new QVBoxLayout(m_tabCadastro);
    QFormLayout* formLayout = new QFormLayout();
    
    m_cadCliente = new QLineEdit();
    m_cadContato = new QLineEdit();
    m_cadDescricao = new QTextEdit();
    m_cadDescricao->setMaximumHeight(80);
    m_cadMedidas = new QLineEdit();
    m_cadMaterial = new QLineEdit();
    m_cadPrazo = new QDateEdit();
    m_cadPrazo->setDate(QDate::currentDate());
    m_cadPrazo->setCalendarPopup(true);
    m_cadStatus = new QComboBox();
    m_cadStatus->addItems({"Aberta", "Em Produção", "Concluída", "Entregue"});
    
    m_grupoPrioridade = new QButtonGroup(this);
    m_prioAlta = new QRadioButton("Alta");
    m_prioMedia = new QRadioButton("Média");
    m_prioBaixa = new QRadioButton("Baixa");
    m_prioMedia->setChecked(true);
    m_grupoPrioridade->addButton(m_prioAlta);
    m_grupoPrioridade->addButton(m_prioMedia);
    m_grupoPrioridade->addButton(m_prioBaixa);
    
    QHBoxLayout* prioLayout = new QHBoxLayout();
    prioLayout->addWidget(m_prioAlta);
    prioLayout->addWidget(m_prioMedia);
    prioLayout->addWidget(m_prioBaixa);

    m_grupoDificuldade = new QButtonGroup(this);
    m_difAlta = new QRadioButton("Alta");
    m_difMedia = new QRadioButton("Média");
    m_difBaixa = new QRadioButton("Baixa");
    m_difMedia->setChecked(true);
    m_grupoDificuldade->addButton(m_difAlta);
    m_grupoDificuldade->addButton(m_difMedia);
    m_grupoDificuldade->addButton(m_difBaixa);

    QHBoxLayout* difLayout = new QHBoxLayout();
    difLayout->addWidget(m_difAlta);
    difLayout->addWidget(m_difMedia);
    difLayout->addWidget(m_difBaixa);
    
    formLayout->addRow("Cliente:", m_cadCliente);
    formLayout->addRow("Contato:", m_cadContato);
    formLayout->addRow("Descrição do Serviço:", m_cadDescricao);
    formLayout->addRow("Medidas:", m_cadMedidas);
    formLayout->addRow("Material:", m_cadMaterial);
    formLayout->addRow("Prazo de Entrega:", m_cadPrazo);
    formLayout->addRow("Status Inicial:", m_cadStatus);
    formLayout->addRow("Prioridade:", prioLayout);
    formLayout->addRow("Dificuldade:", difLayout);
    
    QPushButton* btnSalvar = new QPushButton("Salvar OS");
    btnSalvar->setStyleSheet("background-color: #4CAF50; color: white; padding: 10px; font-weight: bold;");
    connect(btnSalvar, &QPushButton::clicked, this, &MarmoManagerApp::salvarOS);
    
    layout->addLayout(formLayout);
    layout->addWidget(btnSalvar, 0, Qt::AlignCenter);
}

void MarmoManagerApp::salvarOS() {
    QString cliente = m_cadCliente->text();
    QString contato = m_cadContato->text();
    QString descricao = m_cadDescricao->toPlainText();
    QString medidas = m_cadMedidas->text();
    QString material = m_cadMaterial->text();
    QString prazo = m_cadPrazo->date().toString(Qt::ISODate);
    QString status = m_cadStatus->currentText();
    QString prioridade = m_prioAlta->isChecked() ? "Alta" : (m_prioMedia->isChecked() ? "Média" : "Baixa");
    QString dificuldade = m_difAlta->isChecked() ? "Alta" : (m_difMedia->isChecked() ? "Média" : "Baixa");
    
    try {
        int osId = m_controller.cadastrarOS(cliente, contato, descricao, medidas, material, prazo, status, prioridade, dificuldade);
        QMessageBox::information(this, "Sucesso", QString("Ordem de Serviço #%1 cadastrada com sucesso!").arg(osId));
        clearCadastroForm();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Erro", e.what());
    }
}

void MarmoManagerApp::clearCadastroForm() {
    m_cadCliente->clear();
    m_cadContato->clear();
    m_cadDescricao->clear();
    m_cadMedidas->clear();
    m_cadMaterial->clear();
    m_cadPrazo->setDate(QDate::currentDate());
    m_cadStatus->setCurrentIndex(0);
    m_prioMedia->setChecked(true);
    m_difMedia->setChecked(true);
}

void MarmoManagerApp::setupConsultaTab() {
    QVBoxLayout* layout = new QVBoxLayout(m_tabConsulta);
    
    QHBoxLayout* searchLayout = new QHBoxLayout();
    m_searchInput = new QLineEdit();
    m_searchInput->setPlaceholderText("Buscar por Cliente...");
    m_searchStatus = new QComboBox();
    m_searchStatus->addItems({"Todos", "Aberta", "Em Produção", "Concluída", "Entregue"});
    
    m_searchUseDate = new QCheckBox("Filtrar por Data");
    m_searchDataInicio = new QDateEdit();
    m_searchDataInicio->setDate(QDate::currentDate().addDays(-30));
    m_searchDataInicio->setCalendarPopup(true);
    m_searchDataFim = new QDateEdit();
    m_searchDataFim->setDate(QDate::currentDate());
    m_searchDataFim->setCalendarPopup(true);
    
    QPushButton* btnSearch = new QPushButton("Buscar");
    connect(btnSearch, &QPushButton::clicked, this, &MarmoManagerApp::loadOSData);
    
    searchLayout->addWidget(new QLabel("Busca:"));
    searchLayout->addWidget(m_searchInput);
    searchLayout->addWidget(new QLabel("Status:"));
    searchLayout->addWidget(m_searchStatus);
    searchLayout->addWidget(m_searchUseDate);
    searchLayout->addWidget(new QLabel("De:"));
    searchLayout->addWidget(m_searchDataInicio);
    searchLayout->addWidget(new QLabel("Até:"));
    searchLayout->addWidget(m_searchDataFim);
    searchLayout->addWidget(btnSearch);
    
    layout->addLayout(searchLayout);
    
    m_osTable = new QTableWidget();
    m_osTable->setColumnCount(10);
    m_osTable->setHorizontalHeaderLabels({"ID", "Cliente", "Contato", "Medidas", "Material", "Abertura", "Prazo", "Status", "Prioridade", "Dificuldade"});
    m_osTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_osTable->setSelectionBehavior(QTableWidget::SelectRows);
    m_osTable->setEditTriggers(QTableWidget::NoEditTriggers);
    
    layout->addWidget(m_osTable);
    
    QHBoxLayout* actionLayout = new QHBoxLayout();
    m_updateStatusCombo = new QComboBox();
    m_updateStatusCombo->addItems({"Aberta", "Em Produção", "Concluída", "Entregue"});
    
    m_updateResponsavel = new QLineEdit();
    m_updateResponsavel->setPlaceholderText("Seu Nome (Responsável)");
    
    QPushButton* btnUpdate = new QPushButton("Atualizar Status da OS Selecionada");
    connect(btnUpdate, &QPushButton::clicked, this, &MarmoManagerApp::atualizarStatus);
    
    QPushButton* btnHistory = new QPushButton("Ver Histórico de Status");
    connect(btnHistory, &QPushButton::clicked, this, &MarmoManagerApp::verHistorico);
    
    actionLayout->addWidget(new QLabel("Novo Status:"));
    actionLayout->addWidget(m_updateStatusCombo);
    actionLayout->addWidget(new QLabel("Resp:"));
    actionLayout->addWidget(m_updateResponsavel);
    actionLayout->addWidget(btnUpdate);
    actionLayout->addWidget(btnHistory);
    
    layout->addLayout(actionLayout);
}

void MarmoManagerApp::loadOSData() {
    QString buscaCliente = m_searchInput->text().trimmed();
    QString buscaStatus = m_searchStatus->currentText();
    QString dataIni = "";
    QString dataFim = "";
    
    if (m_searchUseDate->isChecked()) {
        dataIni = m_searchDataInicio->date().toString(Qt::ISODate);
        dataFim = m_searchDataFim->date().toString(Qt::ISODate);
    }
    
    QVector<OSData> rows = m_controller.listarOS(buscaCliente, buscaStatus, dataIni, dataFim);
    
    m_osTable->setRowCount(rows.size());
    for (int i = 0; i < rows.size(); ++i) {
        const auto& os = rows[i];
        m_osTable->setItem(i, 0, new QTableWidgetItem(QString::number(os.id)));
        m_osTable->setItem(i, 1, new QTableWidgetItem(os.cliente));
        m_osTable->setItem(i, 2, new QTableWidgetItem(os.contato));
        m_osTable->setItem(i, 3, new QTableWidgetItem(os.medidas));
        m_osTable->setItem(i, 4, new QTableWidgetItem(os.material));
        m_osTable->setItem(i, 5, new QTableWidgetItem(os.dataAbertura));
        m_osTable->setItem(i, 6, new QTableWidgetItem(os.prazo));
        m_osTable->setItem(i, 7, new QTableWidgetItem(os.status));
        m_osTable->setItem(i, 8, new QTableWidgetItem(os.prioridade));
        m_osTable->setItem(i, 9, new QTableWidgetItem(os.dificuldade));
    }
}

int MarmoManagerApp::getSelectedOsId() const {
    auto selectedRows = m_osTable->selectedItems();
    if (selectedRows.isEmpty()) {
        return -1;
    }
    return m_osTable->item(selectedRows.first()->row(), 0)->text().toInt();
}

QString MarmoManagerApp::getSelectedOsStatus() const {
    auto selectedRows = m_osTable->selectedItems();
    if (selectedRows.isEmpty()) {
        return "";
    }
    return m_osTable->item(selectedRows.first()->row(), 7)->text();
}

void MarmoManagerApp::atualizarStatus() {
    int osId = getSelectedOsId();
    if (osId == -1) {
        QMessageBox::warning(this, "Aviso", "Selecione uma OS na tabela para atualizar o status.");
        return;
    }
    
    QString statusAnterior = getSelectedOsStatus();
    QString novoStatus = m_updateStatusCombo->currentText();
    QString responsavel = m_updateResponsavel->text().trimmed();
    
    try {
        m_controller.atualizarStatusOS(osId, statusAnterior, novoStatus, responsavel);
        QMessageBox::information(this, "Sucesso", QString("Status da OS #%1 atualizado com sucesso!").arg(osId));
        loadOSData();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Aviso", e.what());
    }
}

void MarmoManagerApp::verHistorico() {
    int osId = getSelectedOsId();
    if (osId == -1) {
        QMessageBox::warning(this, "Aviso", "Selecione uma OS na tabela para ver o histórico.");
        return;
    }
    
    StatusHistoryDialog dialog(osId, &m_controller, this);
    dialog.exec();
}

void MarmoManagerApp::onTabChanged(int index) {
    if (index == 1) {
        loadOSData();
    }
}
