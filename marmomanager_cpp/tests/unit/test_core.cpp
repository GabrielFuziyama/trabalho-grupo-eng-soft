#include <QtTest>
#include "database.h"
#include "os_repository.h"
#include "historico_repository.h"

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
};

QTEST_MAIN(TestCore)
#include "test_core.moc"
