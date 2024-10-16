qt_add_executable(testGestureRecognizerHold test_gesturerecognizer_hold.cpp)
add_test(NAME "gesturerecognizer_hold" COMMAND testGestureRecognizerHold)
target_link_libraries(testGestureRecognizerHold PRIVATE
    libgestures
    Qt::Core
    Qt::Test
)