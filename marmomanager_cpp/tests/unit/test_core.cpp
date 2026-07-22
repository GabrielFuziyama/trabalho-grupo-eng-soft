#include <QtTest>
#include "database.h"
#include "os_repository.h"
#include "historico_repository.h"
#include "os_controller.h"
#include "auth_controller.h"
#include <QSqlQuery>
#include <QFileInfo>
class TestCore : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Setup in-memory or test database
        QFile::remove("test_marmomanager_cpp.db");
        Database::getInstance().setDbName("test_marmomanager_cpp.db");
        Database::getInstance().initDb();
    }

    void cleanupTestCase() {
        QFile::remove("test_marmomanager_cpp.db");
    }

    void testSingleton() {
        Database& db1 = Database::getInstance();
        Database& db2 = Database::getInstance();
        QCOMPARE(&db1, &db2);
    }

    void testInitialMasterAuthentication() {
        AuthController controller;
        const auto result = controller.authenticate("master", "Master@123");
        QCOMPARE(result.status, AuthenticationStatus::Success);
        QVERIFY(result.user.isMaster());
        QVERIFY(result.user.isAuthenticated());

        const auto invalid = controller.authenticate("master", "senha-incorreta");
        QCOMPARE(invalid.status, AuthenticationStatus::InvalidCredentials);

        QSqlQuery query(Database::getInstance().getConnection());
        QVERIFY(query.exec("SELECT senha_hash FROM usuarios WHERE username = 'master'"));
        QVERIFY(query.next());
        QVERIFY(query.value(0).toString() != QString("Master@123"));
        QVERIFY(query.value(0).toString().startsWith("v1$"));
    }

    void testRelationalDatabaseSchema() {
        QSqlQuery query(Database::getInstance().getConnection());

        QVERIFY(query.exec("PRAGMA foreign_keys"));
        QVERIFY(query.next());
        QCOMPARE(query.value(0).toInt(), 1);

        QVERIFY(query.exec("SELECT name FROM sqlite_master WHERE type = 'table' "
                           "AND name IN ('usuarios', 'os', 'historico_os')"));
        int tableCount = 0;
        while (query.next()) {
            ++tableCount;
        }
        QCOMPARE(tableCount, 3);

        QVERIFY(query.exec("PRAGMA foreign_key_list(historico_os)"));
        QVERIFY(query.next());
        QCOMPARE(query.value("table").toString(), QString("os"));
        QCOMPARE(query.value("from").toString(), QString("os_id"));
        QCOMPARE(query.value("to").toString(), QString("id"));

        QSqlQuery invalidHistory(Database::getInstance().getConnection());
        invalidHistory.prepare(
            "INSERT INTO historico_os (os_id, status_anterior, status_novo, data_alteracao, responsavel) "
            "VALUES (-999, 'N/A', 'Aberta', '2026-07-22 12:00:00', 'Teste')");
        QVERIFY(!invalidHistory.exec());
    }

    void testDatabasePersistsAfterReconnect() {
        QFileInfo databaseFile("test_marmomanager_cpp.db");
        QVERIFY(databaseFile.exists());
        QVERIFY(databaseFile.size() > 0);

        QSqlDatabase connection = Database::getInstance().getConnection();
        connection.close();

        QSqlDatabase reopened = Database::getInstance().getConnection();
        QVERIFY(reopened.isOpen());
        QCOMPARE(reopened.databaseName(), QString("test_marmomanager_cpp.db"));

        QSqlQuery query(reopened);
        QVERIFY(query.exec("SELECT COUNT(*) FROM usuarios WHERE perfil = 'master'"));
        QVERIFY(query.next());
        QCOMPARE(query.value(0).toInt(), 1);
    }

    void testStandardUserLifecycleAndPermissions() {
        AuthController controller;
        const UserData master = controller.authenticate("master", "Master@123").user;

        const int userId = controller.createStandardUser(
            master, "Funcionário Teste", "func.teste", "Teste123", "Teste123");
        QVERIFY(userId > 0);

        auto login = controller.authenticate("FUNC.TESTE", "Teste123");
        QCOMPARE(login.status, AuthenticationStatus::Success);
        QVERIFY(!login.user.isMaster());

        bool denied = false;
        try {
            controller.createStandardUser(login.user, "Outro Usuário", "outro.user", "Teste123", "Teste123");
        } catch (const std::invalid_argument&) {
            denied = true;
        }
        QVERIFY(denied);

        controller.setUserActive(master, userId, false);
        QCOMPARE(controller.authenticate("func.teste", "Teste123").status,
                 AuthenticationStatus::Inactive);

        controller.resetUserPassword(master, userId, "NovaSenha123", "NovaSenha123");
        controller.setUserActive(master, userId, true);
        login = controller.authenticate("func.teste", "NovaSenha123");
        QCOMPARE(login.status, AuthenticationStatus::Success);

        controller.changeOwnPassword(login.user, "NovaSenha123", "SenhaFinal456", "SenhaFinal456");
        QCOMPARE(controller.authenticate("func.teste", "SenhaFinal456").status,
                 AuthenticationStatus::Success);
    }

    void testCreateOs() {
        OSRepository repo;
        int id = repo.create("C++ Cliente", "123", "Teste C++", "1x1", "Granito", "2026-12-12", "Aberta", "Alta", "Alta", "2026-07-09");
        QVERIFY(id > 0);

        auto rows = repo.fetchAll("C++ Cliente");
        QCOMPARE(rows.size(), 1);
        QCOMPARE(rows[0].cliente, QString("C++ Cliente"));
    }

    void testHistorico() {
        OSRepository osRepo;
        HistoricoRepository histRepo;

        int id = osRepo.create("Hist Cliente", "123", "Teste Hist", "1x1", "Granito", "2026-12-12", "Aberta", "Média", "Baixa", "2026-07-09");
        osRepo.updateStatus(id, "Concluída");
        histRepo.create(id, "Aberta", "Concluída", "2026-07-09 12:00:00", "Admin");

        auto hist = histRepo.getByOsId(id);
        QCOMPARE(hist.size(), 1);
        QCOMPARE(hist[0].statusNovo, QString("Concluída"));
    }
    void testControllerCadastrarSucesso() {
        OSController controller;
        int id = controller.cadastrarOS("Cliente Teste Controller", "11999", "Desc", "2x2", "Granito", "2026-10-10", "Aberta", "Baixa", "Média");
        QVERIFY(id > 0);
        
        auto historico = controller.obterHistorico(id);
        QCOMPARE(historico.size(), 1);
        QCOMPARE(historico[0].statusNovo, QString("Aberta"));
    }

    void testControllerCadastrarFalha() {
        OSController controller;
        bool exceptionThrown = false;
        try {
            controller.cadastrarOS("   ", "11999", "Desc", "2x2", "Granito", "2026-10-10", "Aberta", "Baixa", "Média");
        } catch (const std::invalid_argument& e) {
            exceptionThrown = true;
            QCOMPARE(QString(e.what()), QString("O nome do cliente é obrigatório."));
        }
        QVERIFY(exceptionThrown);
    }

    void testControllerAtualizarStatusSucesso() {
        OSController controller;
        int id = controller.cadastrarOS("Cliente Atualizar", "11999", "Desc", "2x2", "Granito", "2026-10-10", "Aberta", "Baixa", "Média");
        
        controller.atualizarStatusOS(id, "Aberta", "Em Andamento", "Funcionario1");
        
        auto historico = controller.obterHistorico(id);
        QCOMPARE(historico.size(), 2);
        QCOMPARE(historico[0].statusNovo, QString("Em Andamento"));
        QCOMPARE(historico[0].responsavel, QString("Funcionario1"));
    }

    void testControllerAtualizarStatusFalhas() {
        OSController controller;
        int id = controller.cadastrarOS("Cliente Atualizar Falha", "11999", "Desc", "2x2", "Granito", "2026-10-10", "Aberta", "Baixa", "Média");
        
        // Falha 1: Sem responsavel
        bool exceptionThrown1 = false;
        try {
            controller.atualizarStatusOS(id, "Aberta", "Em Andamento", "   ");
        } catch (const std::invalid_argument& e) {
            exceptionThrown1 = true;
        }
        QVERIFY(exceptionThrown1);

        // Falha 2: Mesmo status
        bool exceptionThrown2 = false;
        try {
            controller.atualizarStatusOS(id, "Aberta", "Aberta", "Funcionario1");
        } catch (const std::invalid_argument& e) {
            exceptionThrown2 = true;
        }
        QVERIFY(exceptionThrown2);
    }
};
QTEST_MAIN(TestCore)
#include "test_core.moc"
