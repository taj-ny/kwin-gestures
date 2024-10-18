find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Widgets
)

find_package(KWin REQUIRED COMPONENTS
    kwineffects
)

if(${KWin_VERSION} VERSION_GREATER_EQUAL 6.1.90)
    add_compile_definitions(KWIN_6_2_OR_GREATER)
endif()

set(kwin_gestures_SOURCES
    config/config.cpp
    impl/kwininput.cpp
    impl/kwinwindowinfoprovider.cpp
    effect.cpp
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
