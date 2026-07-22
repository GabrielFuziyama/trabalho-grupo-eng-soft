# MarmoManager

Aplicação desktop em C++17, Qt 5 e SQLite para controlar ordens de serviço de
uma marmoraria, com autenticação master/padrão, histórico de alterações e testes
automatizados.

## Atendimento aos critérios

| Critério | Evidência no projeto |
|---|---|
| Documentação completa | Comentários Doxygen nos contratos públicos, `docs/Doxyfile`, esta documentação e `docs/mainpage.dox` |
| Interfaces gráficas funcionais | Login, cadastro/consulta de OS, histórico, administração de usuários e troca de senha em `src/ui` |
| Banco relacional | SQLite via Qt SQL; tabelas `usuarios`, `os` e `historico_os` criadas em `src/database.cpp` |
| Arquitetura em três camadas | Três bibliotecas compartilhadas definidas explicitamente no `CMakeLists.txt` |
| Aceitação BDD + QTest | Features em `tests/acceptance/features` e automação em `tests/acceptance/test_acceptance_bdd.cpp` |

## Componentes e dependências

```text
marmomanager_app
        |
        v
libmarmomanager_presentation.so   (telas e fluxo de sessão)
        |
        v
libmarmomanager_business.so       (regras e casos de uso)
        |
        v
libmarmomanager_data.so           (SQLite, DAOs e hashes)
        |
        v
    SQLite
```

No Linux, bibliotecas dinâmicas usam `.so`. No Windows, o mesmo tipo de alvo
`SHARED` do CMake gera `.dll`. Os controladores ocultam os repositórios por
implementação privada, e o executável depende somente da apresentação.

Após a compilação, os quatro componentes ficam em `bin/`:

```text
marmomanager_app
libmarmomanager_presentation.so
libmarmomanager_business.so
libmarmomanager_data.so
```

## Compilar e executar

Dependências: compilador C++17, CMake, Qt 5 (`Core`, `Widgets`, `Sql`, `Test`),
driver Qt SQLite e, opcionalmente, Doxygen/Graphviz.

```bash
make
./bin/marmomanager_app
```

Primeiro acesso:

```text
Usuário: master
Senha: Master@123
```

Troque a senha inicial em **Sessão → Alterar minha senha**.

## Banco de dados

Em uso normal, o arquivo fica em:

```text
~/.marmomanager/marmomanager.db
```

O esquema é inicializado de forma idempotente. As principais relações são:

- `usuarios`: credenciais derivadas, perfil e estado de acesso;
- `os`: dados e estado atual da ordem de serviço;
- `historico_os`: transições de estado, responsável e chave estrangeira `os_id`.

## Testes

Execute todas as suítes:

```bash
make test
```

Ou somente os testes de aceitação BDD:

```bash
ctest --test-dir build -R AcceptanceBDDTests --output-on-failure
```

Os cenários legíveis estão em:

- `tests/acceptance/features/autenticacao.feature`;
- `tests/acceptance/features/ordem_servico.feature`.

Os passos Dado/Quando/Então são automatizados com QTest em
`tests/acceptance/test_acceptance_bdd.cpp`.

## Documentação de API

Gere a documentação HTML com:

```bash
cmake --build build --target documentation
```

Depois abra `docs/html/index.html` no navegador.
