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
#include <QAction>
#include <QColor>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <stdexcept>

namespace {
bool requestPassword(QWidget* parent, const QString& title, bool askCurrent,
                     QString& currentPassword, QString& newPassword, QString& confirmation) {
    QDialog dialog(parent);
    dialog.setWindowTitle(title);
    dialog.setModal(true);
    dialog.setMinimumWidth(390);

    auto* layout = new QVBoxLayout(&dialog);
    auto* explanation = new QLabel("A nova senha deve ter ao menos 8 caracteres, com letras e números.");
    explanation->setWordWrap(true);
    layout->addWidget(explanation);

    auto* form = new QFormLayout();
    QLineEdit* currentInput = nullptr;
    if (askCurrent) {
        currentInput = new QLineEdit();
        currentInput->setEchoMode(QLineEdit::Password);
        currentInput->setObjectName("currentPassword");
        form->addRow("Senha atual:", currentInput);
    }

    auto* newInput = new QLineEdit();
    newInput->setEchoMode(QLineEdit::Password);
    newInput->setObjectName("newPassword");
    auto* confirmationInput = new QLineEdit();
    confirmationInput->setEchoMode(QLineEdit::Password);
    confirmationInput->setObjectName("passwordConfirmation");
    form->addRow("Nova senha:", newInput);
    form->addRow("Confirmar senha:", confirmationInput);
    layout->addLayout(form);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    buttons->button(QDialogButtonBox::Ok)->setText("Salvar");
    buttons->button(QDialogButtonBox::Cancel)->setText("Cancelar");
    QObject::connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    layout->addWidget(buttons);

    if (dialog.exec() != QDialog::Accepted) {
        return false;
    }
    currentPassword = currentInput ? currentInput->text() : QString();
    newPassword = newInput->text();
    confirmation = confirmationInput->text();
    return true;
}
}

MarmoManagerApp::MarmoManagerApp(QWidget* parent)
    : MarmoManagerApp(UserData{}, parent) {
}

MarmoManagerApp::MarmoManagerApp(const UserData& authenticatedUser, QWidget* parent)
    : QMainWindow(parent), m_loggedUser(authenticatedUser) {
    setWindowTitle("MarmoManager - Sistema de Controle de OS");
    resize(1050, 650);
    
    m_tabs = new QTabWidget(this);
    m_tabs->setObjectName("mainTabs");
    setCentralWidget(m_tabs);
    
    m_tabCadastro = new QWidget();
    m_tabConsulta = new QWidget();
    m_tabUsuarios = new QWidget();
    m_tabCadastro->setObjectName("serviceOrderRegistrationTab");
    m_tabConsulta->setObjectName("serviceOrderSearchTab");
    m_tabUsuarios->setObjectName("usersTab");
    
    m_tabs->addTab(m_tabCadastro, "Cadastrar OS");
    m_tabs->addTab(m_tabConsulta, "Consultar / Atualizar OS");
    
    setupCadastroTab();
    setupConsultaTab();
    setupUsuariosTab();
    setupSessionMenu();
    applyUserPermissions();
    
    connect(m_tabs, &QTabWidget::currentChanged, this, &MarmoManagerApp::onTabChanged);
}

void MarmoManagerApp::setupSessionMenu() {
    QMenu* sessionMenu = menuBar()->addMenu("Sessão");
    QAction* changePasswordAction = sessionMenu->addAction("Alterar minha senha");
    QAction* logoutAction = sessionMenu->addAction("Sair da conta");
    changePasswordAction->setObjectName("changeOwnPasswordAction");
    logoutAction->setObjectName("logoutAction");

    connect(changePasswordAction, &QAction::triggered, this, &MarmoManagerApp::alterarMinhaSenha);
    connect(logoutAction, &QAction::triggered, this, &MarmoManagerApp::logoutRequested);
}

void MarmoManagerApp::applyUserPermissions() {
    const QString displayName = m_loggedUser.nome.trimmed().isEmpty() ? "Sistema" : m_loggedUser.nome;
    const QString profileLabel = m_loggedUser.isMaster() ? "Master" : "Padrão";
    statusBar()->showMessage(QString("Usuário: %1  |  Perfil: %2").arg(displayName, profileLabel));
    setWindowTitle(QString("MarmoManager - %1").arg(displayName));

    m_updateResponsavel->setText(displayName);
    m_updateResponsavel->setReadOnly(true);
    m_updateResponsavel->setToolTip("O responsável é definido pelo usuário autenticado.");

    const int usersTabIndex = m_tabs->indexOf(m_tabUsuarios);
    if (m_loggedUser.isAuthenticated() && m_loggedUser.isMaster()) {
        if (usersTabIndex == -1) {
            m_tabs->addTab(m_tabUsuarios, "Usuários");
        }
        loadUsers();
    } else if (usersTabIndex != -1) {
        m_tabs->removeTab(usersTabIndex);
    }
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
    m_cadCliente->setObjectName("serviceOrderCustomer");
    m_cadContato->setObjectName("serviceOrderContact");
    m_cadDescricao->setObjectName("serviceOrderDescription");
    m_cadMedidas->setObjectName("serviceOrderMeasurements");
    m_cadMaterial->setObjectName("serviceOrderMaterial");
    m_cadPrazo->setObjectName("serviceOrderDeadline");
    m_cadStatus->setObjectName("serviceOrderInitialStatus");
    
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
    btnSalvar->setObjectName("saveServiceOrderButton");
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
    m_searchInput->setObjectName("serviceOrderSearchInput");
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
    btnSearch->setObjectName("searchServiceOrderButton");
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
    m_osTable->setObjectName("serviceOrdersTable");
    m_osTable->setColumnCount(10);
    m_osTable->setHorizontalHeaderLabels({"ID", "Cliente", "Contato", "Medidas", "Material", "Abertura", "Prazo", "Status", "Prioridade", "Dificuldade"});
    m_osTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_osTable->setSelectionBehavior(QTableWidget::SelectRows);
    m_osTable->setEditTriggers(QTableWidget::NoEditTriggers);
    
    layout->addWidget(m_osTable);
    
    QHBoxLayout* actionLayout = new QHBoxLayout();
    m_updateStatusCombo = new QComboBox();
    m_updateStatusCombo->setObjectName("serviceOrderNewStatus");
    m_updateStatusCombo->addItems({"Aberta", "Em Produção", "Concluída", "Entregue"});
    
    m_updateResponsavel = new QLineEdit();
    m_updateResponsavel->setObjectName("serviceOrderResponsible");
    m_updateResponsavel->setPlaceholderText("Responsável autenticado");
    
    QPushButton* btnUpdate = new QPushButton("Atualizar Status da OS Selecionada");
    btnUpdate->setObjectName("updateServiceOrderStatusButton");
    connect(btnUpdate, &QPushButton::clicked, this, &MarmoManagerApp::atualizarStatus);
    
    QPushButton* btnHistory = new QPushButton("Ver Histórico de Status");
    btnHistory->setObjectName("showServiceOrderHistoryButton");
    connect(btnHistory, &QPushButton::clicked, this, &MarmoManagerApp::verHistorico);
    
    actionLayout->addWidget(new QLabel("Novo Status:"));
    actionLayout->addWidget(m_updateStatusCombo);
    actionLayout->addWidget(new QLabel("Resp:"));
    actionLayout->addWidget(m_updateResponsavel);
    actionLayout->addWidget(btnUpdate);
    actionLayout->addWidget(btnHistory);
    
    layout->addLayout(actionLayout);
}

void MarmoManagerApp::setupUsuariosTab() {
    auto* layout = new QVBoxLayout(m_tabUsuarios);

    auto* createGroup = new QGroupBox("Cadastrar usuário padrão");
    auto* createLayout = new QHBoxLayout(createGroup);
    auto* form = new QFormLayout();

    m_userNome = new QLineEdit();
    m_userNome->setObjectName("userFullName");
    m_userUsername = new QLineEdit();
    m_userUsername->setObjectName("newUsername");
    m_userUsername->setPlaceholderText("Ex.: joao.silva");
    m_userPassword = new QLineEdit();
    m_userPassword->setObjectName("newUserPassword");
    m_userPassword->setEchoMode(QLineEdit::Password);
    m_userPasswordConfirmation = new QLineEdit();
    m_userPasswordConfirmation->setObjectName("newUserPasswordConfirmation");
    m_userPasswordConfirmation->setEchoMode(QLineEdit::Password);

    form->addRow("Nome completo:", m_userNome);
    form->addRow("Usuário:", m_userUsername);
    form->addRow("Senha inicial:", m_userPassword);
    form->addRow("Confirmar senha:", m_userPasswordConfirmation);

    auto* createButton = new QPushButton("Cadastrar usuário");
    createButton->setObjectName("createUserButton");
    createButton->setMinimumWidth(170);
    createButton->setStyleSheet("background-color: #2E7D32; color: white; padding: 9px; font-weight: bold;");
    connect(createButton, &QPushButton::clicked, this, &MarmoManagerApp::cadastrarUsuario);

    createLayout->addLayout(form, 1);
    createLayout->addWidget(createButton, 0, Qt::AlignBottom);
    layout->addWidget(createGroup);

    auto* usersLabel = new QLabel("Usuários cadastrados");
    usersLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin-top: 8px;");
    layout->addWidget(usersLabel);

    m_usersTable = new QTableWidget();
    m_usersTable->setObjectName("usersTable");
    m_usersTable->setColumnCount(5);
    m_usersTable->setHorizontalHeaderLabels({"ID", "Nome", "Usuário", "Perfil", "Estado"});
    m_usersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_usersTable->setSelectionBehavior(QTableWidget::SelectRows);
    m_usersTable->setSelectionMode(QTableWidget::SingleSelection);
    m_usersTable->setEditTriggers(QTableWidget::NoEditTriggers);
    m_usersTable->setColumnHidden(0, true);
    layout->addWidget(m_usersTable);

    auto* actions = new QHBoxLayout();
    auto* refreshButton = new QPushButton("Atualizar lista");
    auto* toggleButton = new QPushButton("Ativar / desativar selecionado");
    auto* resetButton = new QPushButton("Redefinir senha do selecionado");
    refreshButton->setObjectName("refreshUsersButton");
    toggleButton->setObjectName("toggleUserButton");
    resetButton->setObjectName("resetUserPasswordButton");
    connect(refreshButton, &QPushButton::clicked, this, &MarmoManagerApp::loadUsers);
    connect(toggleButton, &QPushButton::clicked, this, &MarmoManagerApp::alternarUsuarioAtivo);
    connect(resetButton, &QPushButton::clicked, this, &MarmoManagerApp::redefinirSenhaUsuario);
    actions->addWidget(refreshButton);
    actions->addStretch();
    actions->addWidget(toggleButton);
    actions->addWidget(resetButton);
    layout->addLayout(actions);
}

void MarmoManagerApp::cadastrarUsuario() {
    try {
        const int id = m_authController.createStandardUser(
            m_loggedUser, m_userNome->text(), m_userUsername->text(),
            m_userPassword->text(), m_userPasswordConfirmation->text());
        QMessageBox::information(this, "Usuário cadastrado",
                                 QString("Usuário padrão #%1 cadastrado com sucesso.").arg(id));
        m_userNome->clear();
        m_userUsername->clear();
        m_userPassword->clear();
        m_userPasswordConfirmation->clear();
        loadUsers();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Não foi possível cadastrar", e.what());
    }
}

void MarmoManagerApp::loadUsers() {
    try {
        const QVector<UserData> users = m_authController.listUsers(m_loggedUser);
        m_usersTable->setRowCount(users.size());
        for (int row = 0; row < users.size(); ++row) {
            const auto& user = users[row];
            m_usersTable->setItem(row, 0, new QTableWidgetItem(QString::number(user.id)));
            m_usersTable->setItem(row, 1, new QTableWidgetItem(user.nome));
            m_usersTable->setItem(row, 2, new QTableWidgetItem(user.username));
            m_usersTable->setItem(row, 3, new QTableWidgetItem(user.isMaster() ? "Master" : "Padrão"));
            auto* stateItem = new QTableWidgetItem(user.ativo ? "Ativo" : "Desativado");
            stateItem->setForeground(user.ativo ? QColor("#1B5E20") : QColor("#B71C1C"));
            m_usersTable->setItem(row, 4, stateItem);
        }
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Erro", e.what());
    }
}

int MarmoManagerApp::getSelectedUserId() const {
    const int row = m_usersTable->currentRow();
    if (row < 0 || !m_usersTable->item(row, 0)) {
        return -1;
    }
    return m_usersTable->item(row, 0)->text().toInt();
}

void MarmoManagerApp::alternarUsuarioAtivo() {
    const int row = m_usersTable->currentRow();
    const int userId = getSelectedUserId();
    if (row < 0 || userId < 0) {
        QMessageBox::warning(this, "Seleção necessária", "Selecione um usuário padrão na tabela.");
        return;
    }

    const bool currentlyActive = m_usersTable->item(row, 4)->text() == "Ativo";
    const QString action = currentlyActive ? "desativar" : "ativar";
    if (QMessageBox::question(this, "Confirmar alteração",
                              QString("Deseja realmente %1 o usuário selecionado?").arg(action))
        != QMessageBox::Yes) {
        return;
    }

    try {
        m_authController.setUserActive(m_loggedUser, userId, !currentlyActive);
        loadUsers();
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Não foi possível alterar", e.what());
    }
}

void MarmoManagerApp::redefinirSenhaUsuario() {
    const int userId = getSelectedUserId();
    if (userId < 0) {
        QMessageBox::warning(this, "Seleção necessária", "Selecione um usuário padrão na tabela.");
        return;
    }

    QString unusedCurrent;
    QString newPassword;
    QString confirmation;
    if (!requestPassword(this, "Redefinir senha", false, unusedCurrent, newPassword, confirmation)) {
        return;
    }

    try {
        m_authController.resetUserPassword(m_loggedUser, userId, newPassword, confirmation);
        QMessageBox::information(this, "Senha redefinida", "A senha do usuário foi redefinida com sucesso.");
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Não foi possível redefinir", e.what());
    }
}

void MarmoManagerApp::alterarMinhaSenha() {
    if (!m_loggedUser.isAuthenticated()) {
        QMessageBox::warning(this, "Sessão inválida", "Faça login para alterar sua senha.");
        return;
    }

    QString currentPassword;
    QString newPassword;
    QString confirmation;
    if (!requestPassword(this, "Alterar minha senha", true,
                         currentPassword, newPassword, confirmation)) {
        return;
    }

    try {
        m_authController.changeOwnPassword(
            m_loggedUser, currentPassword, newPassword, confirmation);
        QMessageBox::information(this, "Senha alterada", "Sua senha foi alterada com sucesso.");
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Não foi possível alterar", e.what());
    }
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
