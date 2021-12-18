# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
#Ressources
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Widgets LinguistTools REQUIRED)

add_library(Qt INTERFACE)

target_include_directories(Qt INTERFACE "${Qt5Core_INCLUDE_DIRS}")
target_include_directories(Qt INTERFACE "${Qt5Widgets_INCLUDE_DIRS}")

target_link_directories(Qt INTERFACE "${Qt5Core_LIBRARIES}")
target_link_directories(Qt INTERFACE "${Qt5Widgets_LIBRARIES}")

target_link_libraries(Qt INTERFACE Qt5::Core Qt5::Widgets)

###############################################################################
# Qt support
###############################################################################

