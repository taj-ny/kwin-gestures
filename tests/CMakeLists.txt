set(testGestureRecognizerHold_SRCS test_gesturerecognizer_hold.cpp)
add_executable(testGestureRecognizerHold ${testGestureRecognizerHold_SRCS})
target_link_libraries(testGestureRecognizerHold
    kwin_gestures
    Qt::Test
)
add_test(NAME "gesturerecognizer_hold" COMMAND testGestureRecognizerHold)
ecm_mark_as_test(testGestureRecognizerHold)