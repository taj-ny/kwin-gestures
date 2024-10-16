#include "test_example.h"

void TestExample::test()
{
    QVERIFY(true);
}

QTEST_MAIN(TestExample)
#include "test_example.moc"