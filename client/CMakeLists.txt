cmake_minimum_required(VERSION 3.7)

project(
    qss-client
    VERSION 1.3.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (NOT (${CMAKE_BUILD_TYPE} MATCHES "Debug"))
    set(CMAKE_CXX_FLAGS -O3)
    # generate qm files
    execute_process(COMMAND bash "translate_generation.sh"
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/client)
else ()
    set(CMAKE_CXX_COMPILER "clang++")
endif ()

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE FORMS "*.ui")
file(GLOB_RECURSE RESOURCES "*.qrc")

find_package(PkgConfig REQUIRED)
set(QT Core Gui Widgets Network DBus Sql) #LinguistTools)
find_package(Qt5 REQUIRED ${QT})

pkg_check_modules(3rd_lib REQUIRED
        dtkwidget
	dframeworkdbus
        dtkcore
        libqrencode
	zbar
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS} ${RESOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${3rd_lib_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/libQss/lib
)
target_link_libraries(${PROJECT_NAME} ${3rd_lib_LIBRARIES} QtShadowsocks)

qt5_use_modules(${PROJECT_NAME} ${QT})
set(CMAKE_INSTALL_PREFIX /usr)

# Install qm files
file(GLOB QM_FILES "translations/*.qm")
install(FILES ${QM_FILES} DESTINATION share/qss-client/translations)

# Install icons
foreach (ICON_SIZE 16 24 32 48 64 96 128 256)
    install(FILES Resources/ssw${ICON_SIZE}.svg
            RENAME dpss.svg
            DESTINATION share/icons/hicolor/${ICON_SIZE}x${ICON_SIZE}/apps)
endforeach ()

# Install bin
install(TARGETS qss-client DESTINATION bin)

# Install desktop shortcuts
install(FILES ../dpss.desktop DESTINATION share/applications)
