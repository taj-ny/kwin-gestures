set(kwin_gestures_SOURCES
    gestureseffect.cpp
    inputfilter.cpp
    main.cpp
    virtualkeyboard.cpp
    config/config.cpp
    gesturerecognizer.cpp
    gestures/condition.cpp
    gestures/gesture.cpp
    gestures/holdgesture.cpp
    gestures/pinchgesture.cpp
    gestures/swipegesture.cpp
    gestures/actions/action.cpp
    gestures/actions/command.cpp
    gestures/actions/globalshortcut.cpp
    gestures/actions/keysequence.cpp
)

add_library(kwin_gestures MODULE ${kwin_gestures_SOURCES})
target_link_libraries(kwin_gestures
    KWin::kwin
)

install(TARGETS kwin_gestures DESTINATION ${KDE_INSTALL_PLUGINDIR}/kwin/effects/plugins)
