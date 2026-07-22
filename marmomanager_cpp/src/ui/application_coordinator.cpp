#include "application_coordinator.h"

#include "application_controller.h"
#include "login_dialog.h"
#include "main_window.h"

#include <QApplication>

ApplicationCoordinator::ApplicationCoordinator(QObject* parent)
    : QObject(parent) {}

void ApplicationCoordinator::start(QApplication& application) {
    m_application = &application;
    ApplicationController().initialize();
    showLogin();
}

void ApplicationCoordinator::showLogin() {
    auto* login = new LoginDialog();
    login->setAttribute(Qt::WA_DeleteOnClose);

    connect(login, &QDialog::accepted, this, [this, login]() {
        const UserData user = login->authenticatedUser();
        showMainWindow(user);
        login->deleteLater();
    });
    connect(login, &QDialog::rejected, m_application, &QApplication::quit);
    login->show();
}

void ApplicationCoordinator::showMainWindow(const UserData& user) {
    auto* window = new MarmoManagerApp(user);
    window->setAttribute(Qt::WA_DeleteOnClose);

    connect(window, &MarmoManagerApp::logoutRequested, this, [this, window]() {
        window->hide();
        showLogin();
        window->deleteLater();
    });
    window->show();
}
