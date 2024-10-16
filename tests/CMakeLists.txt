set(testExample_SRCS test_example.cpp)
add_executable(testExample ${testExample_SRCS})
target_link_libraries(testExample
    kwin_gestures
    Qt::Test
)
add_test(NAME "test" COMMAND testExample)
ecm_mark_as_test(testExample)