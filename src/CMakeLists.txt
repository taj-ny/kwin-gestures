set(kwin_gestures_SOURCES
    gestureseffect.cpp
    inputfilter.cpp
    main.cpp
    virtualkeyboard.cpp
    config/config.cpp
    gesturerecognizer.cpp
    gestures/gesture.cpp
    gestures/pinchgesture.cpp
    gestures/swipegesture.cpp
    gestures/actions/action.cpp
    gestures/actions/command.cpp
    gestures/actions/globalshortcut.cpp
    gestures/actions/keysequence.cpp
)

add_library(kwin_gestures MODULE ${kwin_gestures_SOURCES})
target_link_libraries(kwin_gestures
    kwin
        KWin::kwin

    KF6::ConfigGui
        KF6::GlobalAccel

    KDecoration2::KDecoration

    K::KGlobalAccelD
)

#qt_add_dbus_interface(kwin_gestures_SOURCES ${KDE_INSTALL_DBUSINTERFACEDIR}/kf6_org.kde.kglobalaccel.Component.xml kglobalaccel_component_interface)

install(TARGETS kwin_gestures DESTINATION ${KDE_INSTALL_PLUGINDIR}/kwin/effects/plugins)
