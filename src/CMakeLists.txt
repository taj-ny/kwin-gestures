if (BUILD_KWIN_EFFECT)
    add_subdirectory(kwin)
endif()

find_package(yaml-cpp REQUIRED)

set(libinputactions_SRCS
    libinputactions/actions/action.cpp
    libinputactions/actions/actiongroup.cpp
    libinputactions/actions/command.cpp
    libinputactions/actions/plasmaglobalshortcut.cpp
    libinputactions/actions/input.cpp
    libinputactions/actions/oneactiongroup.cpp
    libinputactions/conditions/callbackcondition.cpp
    libinputactions/conditions/condition.cpp
    libinputactions/conditions/conditiongroup.cpp
    libinputactions/conditions/legacycondition.cpp
    libinputactions/handlers/motiontriggerhandler.cpp
    libinputactions/handlers/mousetriggerhandler.cpp
    libinputactions/handlers/multitouchmotiontriggerhandler.cpp
    libinputactions/handlers/touchpadtriggerhandler.cpp
    libinputactions/handlers/triggerhandler.cpp
    libinputactions/triggers/directionalmotiontrigger.cpp
    libinputactions/triggers/motiontrigger.cpp
    libinputactions/triggers/presstrigger.cpp
    libinputactions/triggers/stroketrigger.cpp
    libinputactions/triggers/trigger.cpp
    libinputactions/globals.h
    libinputactions/input.cpp
    libinputactions/range.cpp
    libinputactions/windowinfoprovider.cpp
)

add_library(libinputactions STATIC ${libinputactions_SRCS})
target_link_libraries(libinputactions PRIVATE
    Qt6::Core
    Qt6::DBus
)
target_compile_definitions(libinputactions PUBLIC TEST_VIRTUAL=$<IF:$<BOOL:${BUILD_TESTS}>,virtual,>)
target_include_directories(libinputactions PRIVATE libinputactions)
set_target_properties(libinputactions PROPERTIES PREFIX "")

target_link_libraries(libinputactions PRIVATE yaml-cpp)
target_compile_options(libinputactions PUBLIC -fexceptions -fPIC)