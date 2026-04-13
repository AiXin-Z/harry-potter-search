cmake_minimum_required(VERSION 3.16)

project(HarryPotterSearch VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(HarryPotterSearch
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        HP2--Harry_Potter_and_the_Chamber_of_Secrets_Book_2_.txt
        HP7--Harry_Potter_and_the_Deathly_Hallows_Book_7_.txt
        bmsearch.h bmsearch.cpp
        HP0_Harry_Potter_Prequel.txt
        HP3_Harry_Potter_and_the_Prisoner_of_Azkaban.txt
        HP4_Harry_Potter_and_the_Goblet_of_Fire.txt
        HP6_Harry_Potter_and_the_Half_Blood_Prince.txt
        Quidditch_Through_the_Ages.txt
        The_Tales_of_Beedle_the_Bard.txt

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET HarryPotterSearch APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(HarryPotterSearch SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(HarryPotterSearch
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(HarryPotterSearch PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.HarryPotterSearch)
endif()
set_target_properties(HarryPotterSearch PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS HarryPotterSearch
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(HarryPotterSearch)
endif()
