#include "test_gesture.h"

namespace libinputactions
{

void TestGesture::thresholdReached_data()
{
    QTest::addColumn<qreal>("threshold");
    QTest::addColumn<qreal>("accumulatedDelta");
    QTest::addColumn<bool>("result");

    QTest::newRow("exact") << static_cast<qreal>(10) << static_cast<qreal>(10) << true;
    QTest::newRow("greater") << static_cast<qreal>(10) << static_cast<qreal>(11) << true;
    QTest::newRow("lesser") << static_cast<qreal>(10) << static_cast<qreal>(9) << false;
    QTest::newRow("negative exact") << static_cast<qreal>(-10) << static_cast<qreal>(-10) << true;
    QTest::newRow("negative greater") << static_cast<qreal>(-10) << static_cast<qreal>(-9) << true;
    QTest::newRow("negative lesser") << static_cast<qreal>(-10) << static_cast<qreal>(-11) << false;
}

void TestGesture::thresholdReached()
{
    QFETCH(qreal, threshold);
    QFETCH(qreal, accumulatedDelta);
    QFETCH(bool, result);

    auto gesture = std::make_shared<Gesture>();
    gesture->setThreshold(threshold);

    QCOMPARE(gesture->thresholdReached(accumulatedDelta), result);
}

}

QTEST_MAIN(libinputactions::TestGesture)
#include "test_gesture.moc"