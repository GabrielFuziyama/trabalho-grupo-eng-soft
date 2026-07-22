# language: pt
Funcionalidade: Autenticação e autorização por perfil
  Como responsável pela marmoraria
  Quero controlar o acesso ao MarmoManager
  Para proteger as ordens de serviço e a gestão de usuários

  Cenário: Master acessa a administração de usuários
    Dado que o banco foi inicializado com o usuário master
    Quando o master informa credenciais válidas
    Então o acesso deve ser autorizado
    E a área de administração de usuários deve estar disponível

  Cenário: Usuário desativado não consegue entrar
    Dado que existe um usuário padrão desativado
    Quando ele informa suas credenciais corretas
    Então o acesso deve ser recusado
    E a tela deve informar que o usuário está desativado

  Cenário: Master cadastra um usuário padrão
    Dado que o master está autenticado
    Quando ele cadastra um usuário padrão válido
    Então o novo usuário deve conseguir autenticar-se
