add_subdirectory(kwin)

set(libgestures_SRCS
    libgestures/libgestures/actions/action.cpp
    libgestures/libgestures/actions/command.cpp
    libgestures/libgestures/actions/kdeglobalshortcut.cpp
    libgestures/libgestures/actions/keysequence.cpp
    libgestures/libgestures/gestures/gesture.cpp
    libgestures/libgestures/gestures/gesturerecognizer.cpp
    libgestures/libgestures/gestures/holdgesture.h
    libgestures/libgestures/gestures/pinchgesture.cpp
    libgestures/libgestures/gestures/swipegesture.cpp
    libgestures/libgestures/condition.cpp
    libgestures/libgestures/input.h
    libgestures/libgestures/windowinfoprovider.cpp
)

add_library(libgestures STATIC ${libgestures_SRCS})
target_link_libraries(libgestures PRIVATE
    Qt6::Core
    Qt6::DBus
)
target_include_directories(libgestures PUBLIC libgestures)