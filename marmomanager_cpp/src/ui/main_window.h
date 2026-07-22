/**
 * @file main_window.h
 * @brief Cabeçalho para a view principal MarmoManagerApp.
 */

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QTableWidget>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>
#include "os_controller.h"
#include "auth_controller.h"

/**
 * @class MarmoManagerApp
 * @brief Classe principal de Interface Gráfica (View) para a aplicação.
 */
class MarmoManagerApp : public QMainWindow {
    Q_OBJECT
    friend class TestGUI;

public:
    /**
     * @brief Construtor auxiliar sem sessão, utilizado por testes isolados da view.
     * @param parent Widget Qt proprietário.
     */
    explicit MarmoManagerApp(QWidget* parent = nullptr);

    /**
     * @brief Cria a janela para um usuário autenticado e aplica seu perfil.
     * @param authenticatedUser Dados seguros da sessão atual.
     * @param parent Widget Qt proprietário.
     */
    explicit MarmoManagerApp(const UserData& authenticatedUser, QWidget* parent = nullptr);

signals:
    /** Emitido quando o usuário solicita encerrar somente a sessão atual. */
    void logoutRequested();

private slots:
    void salvarOS();
    void loadOSData();
    void atualizarStatus();
    void verHistorico();
    void onTabChanged(int index);
    void cadastrarUsuario();
    void loadUsers();
    void alternarUsuarioAtivo();
    void redefinirSenhaUsuario();
    void alterarMinhaSenha();

private:
    void setupCadastroTab();
    void setupConsultaTab();
    void setupUsuariosTab();
    void setupSessionMenu();
    void applyUserPermissions();
    void clearCadastroForm();
    int getSelectedOsId() const;
    QString getSelectedOsStatus() const;
    int getSelectedUserId() const;

    OSController m_controller;
    AuthController m_authController;
    UserData m_loggedUser;

    QTabWidget* m_tabs;
    QWidget* m_tabCadastro;
    QWidget* m_tabConsulta;
    QWidget* m_tabUsuarios;

    // Cadastro form widgets
    QLineEdit* m_cadCliente;
    QLineEdit* m_cadContato;
    QTextEdit* m_cadDescricao;
    QLineEdit* m_cadMedidas;
    QLineEdit* m_cadMaterial;
    QDateEdit* m_cadPrazo;
    QComboBox* m_cadStatus;

    QButtonGroup* m_grupoPrioridade;
    QRadioButton* m_prioAlta;
    QRadioButton* m_prioMedia;
    QRadioButton* m_prioBaixa;

    QButtonGroup* m_grupoDificuldade;
    QRadioButton* m_difAlta;
    QRadioButton* m_difMedia;
    QRadioButton* m_difBaixa;

    // Consulta form widgets
    QLineEdit* m_searchInput;
    QComboBox* m_searchStatus;
    QCheckBox* m_searchUseDate;
    QDateEdit* m_searchDataInicio;
    QDateEdit* m_searchDataFim;
    QTableWidget* m_osTable;
    QComboBox* m_updateStatusCombo;
    QLineEdit* m_updateResponsavel;

    // Master-only user management widgets
    QLineEdit* m_userNome;
    QLineEdit* m_userUsername;
    QLineEdit* m_userPassword;
    QLineEdit* m_userPasswordConfirmation;
    QTableWidget* m_usersTable;
};

#endif // MAIN_WINDOW_H
