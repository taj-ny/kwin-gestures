set(libgestures_SRCS
    libgestures/actions/action.cpp
    libgestures/actions/actiongroup.cpp
    libgestures/actions/command.cpp
    libgestures/actions/plasmaglobalshortcut.cpp
    libgestures/actions/input.cpp
    libgestures/actions/oneactiongroup.cpp
    libgestures/conditions/callbackcondition.cpp
    libgestures/conditions/condition.cpp
    libgestures/conditions/conditiongroup.cpp
    libgestures/conditions/legacycondition.cpp
    libgestures/handlers/motiontriggerhandler.cpp
    libgestures/handlers/mousetriggerhandler.cpp
    libgestures/handlers/multitouchmotiontriggerhandler.cpp
    libgestures/handlers/touchpadtriggerhandler.cpp
    libgestures/handlers/triggerhandler.cpp
    libgestures/triggers/directionalmotiontrigger.cpp
    libgestures/triggers/motiontrigger.cpp
    libgestures/triggers/presstrigger.cpp
    libgestures/triggers/stroketrigger.cpp
    libgestures/triggers/trigger.cpp
    libgestures/globals.h
    libgestures/input.cpp
    libgestures/range.cpp
    libgestures/windowinfoprovider.cpp
)

add_library(libgestures STATIC ${libgestures_SRCS})
target_link_libraries(libgestures PRIVATE
    Qt6::Core
    Qt6::DBus
)
target_compile_definitions(libgestures PUBLIC TEST_VIRTUAL=$<IF:$<BOOL:${BUILD_TESTS}>,virtual,>)
target_include_directories(libgestures PUBLIC libgestures)

find_package(yaml-cpp REQUIRED)
target_link_libraries(libgestures PRIVATE yaml-cpp)
target_compile_options(libgestures PUBLIC -fexceptions -fPIC)

if (BUILD_KWIN_EFFECT)
    add_subdirectory(kwin)
endif()