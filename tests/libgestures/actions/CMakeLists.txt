set(testAction_SRCS
    test_action.cpp
    ../mockwindowinfoprovider.cpp
    ../mockwindowinfoprovider.h
)
qt_add_executable(testAction ${testAction_SRCS})
add_test(NAME "action" COMMAND testAction)
target_link_libraries(testAction PRIVATE
    libgestures
    Qt::Core
    Qt::Test
)