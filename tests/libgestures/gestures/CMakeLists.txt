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