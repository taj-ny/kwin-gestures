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

set(testGestureRecognizer_SRCS
    test_gesturerecognizer.cpp
)
qt_add_executable(testGestureRecognizer ${testGestureRecognizer_SRCS})
add_test(NAME "gesturerecognizer" COMMAND testGestureRecognizer)
target_link_libraries(testGestureRecognizer PRIVATE
    libgestures
    Qt::Core
    Qt::Test
)