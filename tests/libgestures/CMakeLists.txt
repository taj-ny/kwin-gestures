add_subdirectory(actions)
add_subdirectory(gestures)

set(testCondition_SRCS
    test_condition.cpp
    mockwindowinfoprovider.cpp
)
qt_add_executable(testCondition ${testCondition_SRCS})
add_test(NAME "condition" COMMAND testCondition)
target_link_libraries(testCondition PRIVATE
    libgestures
    Qt::Core
    Qt::Test
)