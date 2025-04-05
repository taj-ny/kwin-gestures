enable_testing()

find_package(GTest REQUIRED)
find_package(Qt6Test ${QT_MIN_VERSION} REQUIRED)

add_subdirectory(libgestures)
include_directories(libgestures)