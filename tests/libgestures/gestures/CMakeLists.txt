set(testGesture_SRCS
    test_gesture.cpp
)
qt_add_executable(testGesture ${testGesture_SRCS})
add_test(NAME "gesture" COMMAND testGesture)
target_link_libraries(testGesture PRIVATE
    libgestures
    Qt::Core
    Qt::Test
)

set(testGestureRecognizerHold_SRCS
    test_gesturerecognizer_hold.cpp
    test_gesturerecognizer_shared.cpp
)
qt_add_executable(testGestureRecognizerHold ${testGestureRecognizerHold_SRCS})
add_test(NAME "gesturerecognizer_hold" COMMAND testGestureRecognizerHold)
target_link_libraries(testGestureRecognizerHold PRIVATE
    libgestures
    Qt::Core
    Qt::Test
)