#include "application_controller.h"

#include "database.h"

void ApplicationController::initialize() const {
    Database::getInstance().initDb();
}
