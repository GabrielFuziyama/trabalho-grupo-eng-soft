
#define UNIT_TEST_5

/*******************************************************************************************/

// SIMPLEST UNIT TEST
#ifdef UNIT_TEST_1
#include <QtTest/QtTest>

class TestQString: public QObject
{
    Q_OBJECT
private slots:
    void toUpper();
};

void TestQString::toUpper()
{
    QString str = "Hello";
    //QVERIFY(str.toUpper() == "HELLO"); // they are simular
    QCOMPARE(str.toUpper(), QString("HELLO")); // but this sends more verbose output to the test log

    // HOW TO VERIFY MULTIPLE "TEST SCENARIOS"?
    // How to avoid this:
    QCOMPARE(QString("hello").toUpper(), QString("HELLO"));
    QCOMPARE(QString("Hello").toUpper(), QString("HELLO"));
    QCOMPARE(QString("HellO").toUpper(), QString("HELLO"));
    QCOMPARE(QString("HELLO").toUpper(), QString("HELLO"));
}

QTEST_MAIN(TestQString)
#include "unitTest.moc"

#endif

/*******************************************************************************************/

// MULTIPLE TEST SCENARIOS
#ifdef UNIT_TEST_2

#include <QtTest/QtTest>

class TestQString: public QObject
{
    Q_OBJECT

private slots:
    void toUpper_data();
    void toUpper();
};

void TestQString::toUpper_data()
{
    QTest::addColumn<QString>("string");
    QTest::addColumn<QString>("result");

    QTest::newRow("all lower") << "hello" << "HELLO";
    QTest::newRow("mixed")     << "Hello" << "HELLO";
    QTest::newRow("all upper") << "HELLO" << "HELLO";
}

void TestQString::toUpper()
{
    QFETCH(QString, string);
    QFETCH(QString, result);

    QCOMPARE(string.toUpper(), result);
}

QTEST_MAIN(TestQString)
#include "unitTest.moc"

#endif

/*******************************************************************************************/


// CHALLENGING TEST SCENARIO: Simulating GUI Events
#ifdef UNIT_TEST_3

#include <QtWidgets>
#include <QtTest/QtTest>

class TestGui: public QObject
{
    Q_OBJECT

private slots:
    void testGui();

};

void TestGui::testGui()
{
    QLineEdit lineEdit;

    QTest::keyClicks(&lineEdit, "hello world");

    QCOMPARE(lineEdit.text(), QString("hello world"));
}

QTEST_MAIN(TestGui)
#include "unitTest.moc"

#endif

/*******************************************************************************************/

// MULTIPLE CHALLENGING TEST SCENARIO: Replaying GUI Events
#ifdef UNIT_TEST_4

#include <QtWidgets>
#include <QtTest/QtTest>

class TestGui: public QObject
{
    Q_OBJECT

private slots:
    void testGui_data();
    void testGui();
};

void TestGui::testGui_data()
{
    QTest::addColumn<QTestEventList>("events");
    QTest::addColumn<QString>("expected");

    QTestEventList list1;
    list1.addKeyClick('a');
    QTest::newRow("char") << list1 << "a";

    QTestEventList list2;
    list2.addKeyClick('a');
    list2.addKeyClick(Qt::Key_Backspace);
    QTest::newRow("there and back again") << list2 << "";
}

void TestGui::testGui()
{
    QFETCH(QTestEventList, events);
    QFETCH(QString, expected);

    QLineEdit lineEdit;

    events.simulate(&lineEdit);

    QCOMPARE(lineEdit.text(), expected);
}

QTEST_MAIN(TestGui)
#include "unitTest.moc"

#endif

/*******************************************************************************************/


// BENCHMARKING
#ifdef UNIT_TEST_5


#include <QtWidgets>
#include <qtest.h>

class TestBenchmark : public QObject
{
    Q_OBJECT
private slots:
    void simple();
    void multiple_data();
    void multiple();
    void series_data();
    void series();
};

void TestBenchmark::simple()
{
    QString str1 = QLatin1String("This is a test string");
    QString str2 = QLatin1String("This is a test string");

    QCOMPARE(str1.localeAwareCompare(str2), 0);

    QBENCHMARK {
        str1.localeAwareCompare(str2);
    }
}

void TestBenchmark::multiple_data()
{
    QTest::addColumn<bool>("useLocaleCompare");
    QTest::newRow("locale aware compare") << true;
    QTest::newRow("standard compare") << false;
}

void TestBenchmark::multiple()
{
    QFETCH(bool, useLocaleCompare);
    QString str1 = QLatin1String("This is a test string");
    QString str2 = QLatin1String("This is a test string");

    int result;
    if (useLocaleCompare) {
        QBENCHMARK {
            result = str1.localeAwareCompare(str2);
        }
    } else {
        QBENCHMARK {
            result = (str1 == str2);
        }
    }
    Q_UNUSED(result);
}

void TestBenchmark::series_data()
{
    QTest::addColumn<bool>("useLocaleCompare");
    QTest::addColumn<int>("stringSize");

    for (int i = 1; i < 10000; i += 2000) {
        QByteArray size = QByteArray::number(i);
        QTest::newRow(("locale aware compare--" + size).constData()) << true << i;
        QTest::newRow(("standard compare--" + size).constData()) << false << i;
    }


    // Skiping all series tests
    //QSKIP(">>>Skipping all series tests...");
}

void TestBenchmark::series()
{

    QFETCH(bool, useLocaleCompare);
    QFETCH(int, stringSize);

    QString str1 = QString().fill('A', stringSize);
    QString str2 = QString().fill('A', stringSize);
    int result;
    if (useLocaleCompare) {

        // Skiping some tests
        // QSKIP(">>>Skipping only useLocaleCompare...");

        QBENCHMARK {
            result = str1.localeAwareCompare(str2);
        }
    } else {
        QBENCHMARK {
            result = (str1 == str2);
        }
    }
    Q_UNUSED(result);
}


QTEST_MAIN(TestBenchmark)
#include "unitTest.moc"
#endif

