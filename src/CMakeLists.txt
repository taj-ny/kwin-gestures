set(libgestures_SRCS
    libgestures/libgestures/actions/action.cpp
    libgestures/libgestures/actions/command.cpp
    libgestures/libgestures/actions/kdeglobalshortcut.cpp
    libgestures/libgestures/actions/input.cpp
    libgestures/libgestures/gestures/gesture.cpp
    libgestures/libgestures/gestures/gesturerecognizer.cpp
    libgestures/libgestures/gestures/holdgesture.h
    libgestures/libgestures/gestures/pinchgesture.cpp
    libgestures/libgestures/gestures/swipegesture.cpp
    libgestures/libgestures/condition.cpp
    libgestures/libgestures/input.cpp
    libgestures/libgestures/windowinfoprovider.cpp
)

add_library(libgestures STATIC ${libgestures_SRCS})
target_link_libraries(libgestures PRIVATE
    Qt6::Core
    Qt6::DBus
)


find_package(yaml-cpp REQUIRED)
target_link_libraries(libgestures PRIVATE yaml-cpp)
target_compile_options(libgestures PUBLIC -fexceptions -fPIC)

target_include_directories(libgestures PUBLIC libgestures)

if (BUILD_KWIN_EFFECT)
    add_subdirectory(kwin)
endif()