set(libgestures_SRCS
    libgestures/libgestures/actions/action.cpp
    libgestures/libgestures/actions/actiongroup.cpp
    libgestures/libgestures/actions/command.cpp
    libgestures/libgestures/actions/plasmaglobalshortcut.cpp
    libgestures/libgestures/actions/input.cpp
    libgestures/libgestures/actions/oneactiongroup.cpp
    libgestures/libgestures/conditions/callbackcondition.cpp
    libgestures/libgestures/conditions/condition.cpp
    libgestures/libgestures/conditions/conditiongroup.cpp
    libgestures/libgestures/conditions/legacycondition.cpp
    libgestures/libgestures/handlers/motiontriggerhandler.cpp
    libgestures/libgestures/handlers/mousetriggerhandler.cpp
    libgestures/libgestures/handlers/multitouchmotiontriggerhandler.cpp
    libgestures/libgestures/handlers/touchpadtriggerhandler.cpp
    libgestures/libgestures/handlers/triggerhandler.cpp
    libgestures/libgestures/triggers/directionalmotiontrigger.cpp
    libgestures/libgestures/triggers/motiontrigger.cpp
    libgestures/libgestures/triggers/presstrigger.cpp
    libgestures/libgestures/triggers/stroketrigger.cpp
    libgestures/libgestures/triggers/trigger.cpp
    libgestures/libgestures/globals.h
    libgestures/libgestures/input.cpp
    libgestures/libgestures/range.cpp
    libgestures/libgestures/windowinfoprovider.cpp
)

add_library(libgestures STATIC ${libgestures_SRCS})
target_link_libraries(libgestures PRIVATE
    Qt6::Core
    Qt6::DBus
)
target_compile_definitions(libgestures PUBLIC TEST_VIRTUAL=$<IF:$<BOOL:${BUILD_TESTS}>,virtual,>)

find_package(yaml-cpp REQUIRED)
target_link_libraries(libgestures PRIVATE yaml-cpp)
target_compile_options(libgestures PUBLIC -fexceptions -fPIC)

target_include_directories(libgestures PUBLIC libgestures)

if (BUILD_KWIN_EFFECT)
    add_subdirectory(kwin)
endif()