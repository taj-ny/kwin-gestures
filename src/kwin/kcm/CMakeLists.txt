find_package(KF6 REQUIRED COMPONENTS
    CoreAddons
    I18n
    KCMUtils
)

set(kwin_gestures_kcm_SRCS
    kcm.cpp
    kcm.h
    kcm.qrc
)

ki18n_wrap_ui(kwin_gestures_kcm_SRCS kcm.ui)

add_library(kwin_gestures_kcm MODULE ${kwin_gestures_kcm_SRCS})
target_link_libraries(kwin_gestures_kcm
    KF6::KCMUtils
    KF6::CoreAddons
    KF6::I18n
    Qt6::DBus
)

install(TARGETS kwin_gestures_kcm DESTINATION ${KDE_INSTALL_PLUGINDIR}/kwin/effects/configs)

add_compile_definitions(ABOUT_VERSION_STRING="${PROJECT_VERSION}")