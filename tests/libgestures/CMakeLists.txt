include_directories(.)

macro(libgestures_add_test TARGET_NAME)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    qt_add_executable(${TARGET_NAME} ${TEST_SOURCES})
    add_test(NAME "${TARGET_NAME}" COMMAND ${TARGET_NAME})
    target_link_libraries(${TARGET_NAME} PRIVATE
        libgestures
        GTest::gmock_main
        Qt::Core
        Qt::Test
    )
endmacro()

libgestures_add_test(action SOURCES actions/test_action.cpp)
libgestures_add_test(actioninterval SOURCES actions/test_actioninterval.cpp)
libgestures_add_test(conditiongroup SOURCES conditions/test_conditiongroup.cpp)
libgestures_add_test(directionalmotiontrigger SOURCES triggers/test_directionalmotiontrigger.cpp)
libgestures_add_test(motiontrigger SOURCES triggers/test_motiontrigger.cpp)
libgestures_add_test(trigger SOURCES triggers/test_trigger.cpp)
