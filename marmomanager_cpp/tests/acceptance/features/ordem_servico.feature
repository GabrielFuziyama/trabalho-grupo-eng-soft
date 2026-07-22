# language: pt
Funcionalidade: Ciclo de vida de uma ordem de serviço
  Como usuário padrão autenticado
  Quero cadastrar e atualizar ordens de serviço
  Para acompanhar o trabalho da marmoraria com rastreabilidade

  Cenário: Operador cadastra e atualiza uma ordem de serviço
    Dado que existe um operador padrão autenticado
    Quando ele cadastra uma ordem de serviço para um cliente
    E altera o status da ordem para Concluída
    Então a ordem deve permanecer armazenada no SQLite
    E o histórico deve registrar o nome do operador responsável
