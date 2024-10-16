set(libgestures_SRCS
    actions/action.cpp
    actions/command.cpp
    actions/kdeglobalshortcut.cpp
    actions/keysequence.cpp
    gestures/gesture.cpp
    gestures/gesturerecognizer.cpp
    gestures/holdgesture.cpp
    gestures/pinchgesture.cpp
    gestures/swipegesture.cpp
    condition.cpp
    virtualinputdevice.h
    windowdataprovider.cpp
)

add_library(libgestures STATIC ${libgestures_SRCS})
target_link_libraries(libgestures PRIVATE
    Qt6::Core
    Qt6::DBus
)