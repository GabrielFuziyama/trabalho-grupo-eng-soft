#include <QtTest>
#include "database.h"
#include "os_repository.h"
#include "historico_repository.h"
#include "os_controller.h"
class TestCore : public QObject {
    Q_OBJECT

private slots:
    void initTestCase() {
        // Setup in-memory or test database
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
