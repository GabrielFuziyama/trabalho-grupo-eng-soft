#ifndef APPLICATION_CONTROLLER_H
#define APPLICATION_CONTROLLER_H

/**
 * @file application_controller.h
 * @brief Fachada da camada de negócios para inicialização do sistema.
 */

/**
 * @class ApplicationController
 * @brief Expõe à apresentação as operações de ciclo de vida da aplicação.
 *
 * A apresentação não acessa o banco diretamente. Esta fachada mantém a
 * dependência no sentido apresentação -> negócios -> dados.
 */
class ApplicationController {
public:
    /** Inicializa a conexão e aplica o esquema relacional necessário. */
    void initialize() const;
};

#endif // APPLICATION_CONTROLLER_H
