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

/**
 * @class MarmoManagerApp
 * @brief Classe principal de Interface Gráfica (View) para a aplicação.
 */
class MarmoManagerApp : public QMainWindow {
    Q_OBJECT
    friend class TestGUI;

public:
    MarmoManagerApp(QWidget* parent = nullptr);

private slots:
    void salvarOS();
    void loadOSData();
    void atualizarStatus();
    void verHistorico();
    void onTabChanged(int index);

private:
    void setupCadastroTab();
    void setupConsultaTab();
    void clearCadastroForm();
    int getSelectedOsId() const;
    QString getSelectedOsStatus() const;

    OSController m_controller;

    QTabWidget* m_tabs;
    QWidget* m_tabCadastro;
    QWidget* m_tabConsulta;

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
};

#endif // MAIN_WINDOW_H
