# Roteiro de Apresentação - Sprint 1 (Máximo: 5 Minutos)

**Tempo total estimado:** 4 minutos e 30 segundos (margem de segurança para não ultrapassar 5 min).
**Velocidade recomendada:** Falar de forma calma, clara e contínua (aprox. 130 palavras por minuto).

---

## Slide 1: Abertura (0:00 - 0:15)
**O que mostrar no slide:**
Capa da apresentação com o nome do projeto "MarmoManager (Versão C++ / Qt5)", nome da disciplina (Engenharia de Software) e o nome dos integrantes do Grupo Fornecedor (Keu, Luiz, Rafael).

**Fala do Narrador:**
> "Olá a todos. Nós somos o grupo fornecedor composto por Keu, Luiz e Rafael. Neste vídeo, apresentaremos as entregas e métricas de qualidade do nosso sistema MarmoManager, desenvolvido em C++ com o framework Qt, referentes à nossa primeira Sprint."

---

## Slide 2: Boards e Interações Diárias (0:15 - 0:50)
**O que mostrar no slide:**
*Montagem (print)* com a evolução do Kanban (Trello, GitHub Projects ou GitLab Boards). Mostre um print do Início da Sprint (com tarefas no 'To Do') e um print do Fim da Sprint (com as tarefas em 'Done').

**Fala do Narrador:**
> "Iniciando pela nossa gestão ágil, nós utilizamos boards diários para acompanhar nossas interações. Neste slide, vocês podem ver a evolução do nosso Backlog: à esquerda, o registro das tarefas no início da interação; e à direita, a conclusão ao final do ciclo. Dividimos o trabalho em pacotes pequenos para garantir entregas contínuas todos os dias."

---

## Slide 3: GitFlow e Equilíbrio de Contribuições (0:50 - 1:30)
**O que mostrar no slide:**
1. O grafo de Commits (Network Graph do GitHub/GitLab), mostrando ramificações de *branches* e *merges*.
2. Um gráfico de pizza ou painel de "Contributors" provando que Keu, Luiz e Rafael comitaram de forma equilibrada.

**Fala do Narrador:**
> "Na gestão de versão, seguimos estritamente o GitFlow. Como é possível observar no grafo de commits, criamos branches de 'feature' independentes que convergiam para a nossa branch 'develop' e, finalmente, para a 'main'. Também garantimos um forte equilíbrio nas contribuições, com todos os três membros (Keu, Luiz e Rafael) participando ativamente do desenvolvimento das lógicas e interfaces."

---

## Slide 4: Arquitetura em C++ e Componentização (DLLs) (1:30 - 2:30)
**O que mostrar no slide:**
Estrutura do `CMakeLists.txt` e a divisão de pastas (`marmomanager_cpp/core` e `marmomanager_cpp/app`). Uma imagem apontando para a geração do `libmarmocore.so`.

**Fala do Narrador:**
> "Entrando nos requisitos de Engenharia de Software, nós migramos e reestruturamos todo o código para C++. 
> Atendendo à exigência de componentização de serviços em DLLs, utilizamos o sistema CMake para dividir a aplicação durante a compilação. Geramos uma Biblioteca Compartilhada, ou Shared Library, chamada `libmarmocore`, que engloba totalmente as lógicas e banco de dados. O nosso executável visual em Qt é uma casca que apenas consome essa biblioteca, garantindo altíssimo desacoplamento."

---

## Slide 5: Padrões de Projeto (Singleton e DAO) e Camadas (2:30 - 3:20)
**O que mostrar no slide:**
1. Print do arquivo `database.h` focando em `static Database& getInstance();` e os construtores privados (Singleton).
2. Print do `os_repository.h` (Interfaces separadas das classes concretas `.cpp`).

**Fala do Narrador:**
> "Para garantir a separação em camadas, utilizamos a arquitetura MVC (Model, View, Controller). Separamos rigorosamente as definições de interfaces (nos arquivos '.h') das implementações concretas (nos arquivos '.cpp').
> Aplicamos também Padrões de Projeto clássicos: Utilizamos o Singleton na nossa classe de Banco de Dados, garantindo que o software compartilhe sempre uma única e exclusiva conexão SQL. E aplicamos o padrão DAO (Data Access Object) nos Repositórios, blindando as telas de terem qualquer contato com instruções SQL diretas."

---

## Slide 6: Boas Práticas e Documentação (Doxygen) (3:20 - 4:00)
**O que mostrar no slide:**
1. Print de um trecho de código mostrando boa endentação, classes em `PascalCase` e funções em `camelCase`.
2. Um print da tela da documentação HTML gerada pelo **Doxygen**.

**Fala do Narrador:**
> "No quesito código limpo, padronizamos as nomenclaturas: nossas classes utilizam PascalCase e os métodos utilizam camelCase, além de endentação rigorosa.
> Todo o nosso código fonte, incluindo as bibliotecas C++, foi comentado seguindo as marcações em Javadoc. Com isso, conseguimos rodar o Doxygen e extrair, de forma cem por cento automatizada, o portal em HTML com a documentação técnica completa do sistema."

---

## Slide 7: Demonstração e Avaliação Final (4:00 - 4:40)
**O que mostrar no slide:**
1. Um pequeno vídeo (inserido no slide) ou uma sequência de prints mostrando a tela principal do *MarmoManager* construída em Qt rodando.
2. Ao lado, a tabela `Avaliacao_Sprint1.xlsx` mostrando as notas 100%.

**Fala do Narrador:**
> "Aqui demonstramos a aplicação funcionando em Qt5, já consumindo nossos controladores em C++. 
> Como entregamos a completude do backlog e atendemos a cem por cento dos critérios de testes automatizados unitários usando o framework QTest, além de implementar todos os Padrões Arquiteturais exigidos pelo grupo cliente, nossa autoavaliação da Sprint nesta planilha atinge a pontuação máxima. 
> Muito obrigado pela atenção."

---
*(Encerra-se o vídeo aos 4 minutos e 40 segundos, garantindo folga)*
