set(kwin_gestures_SOURCES
    config/config.cpp
    impl/kwinvirtualinputdevice.cpp
    impl/kwinwindowdataprovider.cpp
    gestureseffect.cpp
    inputfilter.cpp
    main.cpp
)

add_library(kwin_gestures MODULE ${kwin_gestures_SOURCES})
target_link_libraries(kwin_gestures PRIVATE
    KWin::kwin
    libgestures
    Qt6::Widgets
)

install(TARGETS kwin_gestures DESTINATION ${KDE_INSTALL_PLUGINDIR}/kwin/effects/plugins)
