cmake_minimum_required(VERSION 3.18)
project(ru.kotdath.SDLExample CXX)

find_package (Qt5 COMPONENTS Core Qml Gui Quick LinguistTools REQUIRED)

include(FindPkgConfig)
pkg_search_module(AURORA auroraapp REQUIRED)
pkg_search_module(AURORA auroraapp_i18n REQUIRED)

# Find OpenGL ES 2.0 using pkg-config for Aurora OS
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLESv2 REQUIRED glesv2)

# Find EGL for Wayland (no X11)
pkg_check_modules(EGL REQUIRED egl)

# Add Wayland support
pkg_check_modules(WAYLAND REQUIRED wayland-client wayland-egl)
pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols)

# Set EGL_NO_X11 for Wayland
add_definitions(-DEGL_NO_X11)

# Print debug info
message(STATUS "GLESv2 libraries: ${GLESv2_LIBRARIES}")
message(STATUS "GLESv2 include dirs: ${GLESv2_INCLUDE_DIRS}")
message(STATUS "EGL libraries: ${EGL_LIBRARIES}")
message(STATUS "EGL include dirs: ${EGL_INCLUDE_DIRS}")

# Build SDL2 from local source directory
set(SDL2_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/sdlbin)
set(SDL2_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libsdl)

# Configure SDL2 build for Wayland
message(STATUS "Configuring SDL2 in ${SDL2_BUILD_DIR} for Wayland")

execute_process(
    COMMAND ${CMAKE_COMMAND}
        -G Ninja
        -DCMAKE_MAKE_PROGRAM=/usr/bin/ninja
        -B${SDL2_BUILD_DIR}
        -DCMAKE_BUILD_TYPE=Release
        -DSDL_PULSEAUDIO=ON
        -DSDL_RPATH=OFF
        -DSDL_STATIC=OFF
        -DSDL_WAYLAND=ON
        -DSDL_WAYLAND_SHARED=ON
        -DSDL_X11=OFF
        -DSDL_WAYLAND_LIBDECOR=OFF
        -DSDL_OPENGL=ON
        -DSDL_OPENGLES=ON
        -DCMAKE_INSTALL_PREFIX=${SDL2_INSTALL_DIR}
        -DEGL_NO_X11=1
        ${CMAKE_SOURCE_DIR}/libsdl
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    RESULT_VARIABLE SDL2_CONFIGURE_RESULT
)

if(SDL2_CONFIGURE_RESULT)
    message(FATAL_ERROR "Failed to configure SDL2")
endif()

# Build SDL2 immediately after configuration
message(STATUS "Building SDL2...")
execute_process(
    COMMAND ninja -C ${SDL2_BUILD_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    RESULT_VARIABLE SDL2_BUILD_RESULT
)

if(SDL2_BUILD_RESULT)
    message(FATAL_ERROR "Failed to build SDL2")
endif()

# Create a dummy target for compatibility
add_custom_target(sdl2_build ALL
    COMMAND ${CMAKE_COMMAND} -E echo "SDL2 already built during configuration"
    COMMENT "SDL2 was built during configuration phase"
)

# Create imported target
add_library(SDL2SG::SDL2SG SHARED IMPORTED)
set_target_properties(SDL2SG::SDL2SG PROPERTIES
    IMPORTED_LOCATION ${SDL2_BUILD_DIR}/libSDL2-2.0.so
    INTERFACE_INCLUDE_DIRECTORIES ${SDL2_SOURCE_DIR}/include
)


set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)

set(SHORT_NAME SDLExample)

file(GLOB_RECURSE SOURCES_FILES "src/*.cpp")

file(GLOB_RECURSE HEADER_FILES "src/*.h")

file(GLOB_RECURSE QML_FILES "qml/*.qml")

file(GLOB_RECURSE ICON_FILES "icons/*.png")

file(GLOB TS_FILES "translations/*.ts")

file(GLOB MD_FILES "*.md")

set(TRANSLATION_SOURCES_FILES
    ${SOURCES_FILES}
    ${QML_FILES}
)

set(GEN_TS_PATH ${CMAKE_CURRENT_BINARY_DIR}/gen_translations)

foreach(_ts_file ${TS_FILES})
    file(COPY ${_ts_file} DESTINATION ${GEN_TS_PATH})
endforeach()
file(GLOB GEN_TS_FILES "${GEN_TS_PATH}/*.ts")

add_custom_target(update_translations)
foreach(_ts_file ${GEN_TS_FILES})
    get_filename_component(_ts_name ${_ts_file} NAME_WE)
    add_custom_command(
        OUTPUT ${_ts_file}
        DEPENDS ${TRANSLATION_SOURCES_FILES}
        COMMAND ${Qt5_LUPDATE_EXECUTABLE}
        ARGS ${TRANSLATION_SOURCES_FILES} -ts ${_ts_file}
        COMMAND ${CMAKE_COMMAND} -E copy
        ARGS ${_ts_file} ${CMAKE_CURRENT_SOURCE_DIR}/translations
    )
    add_custom_target(translate_${_ts_name} DEPENDS ${_ts_file})
    add_dependencies(update_translations translate_${_ts_name})
endforeach()

qt5_add_translation(TRANSLATION_QM_FILES ${GEN_TS_FILES})

set(CMAKE_SKIP_RPATH FALSE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/lib")

add_executable(${PROJECT_NAME}
    ${SOURCES_FILES}
    ${HEADER_FILES}
    ${ICON_FILES}
    ${QML_FILES}
    ${TS_FILES}
    ${PROJECT_NAME}.desktop
    rpm/${PROJECT_NAME}.spec
    ${TRANSLATION_QM_FILES}
    ${MD_FILES}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>
)
target_include_directories(${PROJECT_NAME} PRIVATE
    $<BUILD_INTERFACE:
    ${AURORA_INCLUDE_DIRS}
    ${GLESv2_INCLUDE_DIRS}
    ${EGL_INCLUDE_DIRS}
    ${WAYLAND_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Quick
    ${AURORA_LDFLAGS}
    SDL2SG::SDL2SG
    ${GLESv2_LIBRARIES}
    ${EGL_LIBRARIES}
    ${WAYLAND_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY qml
    DESTINATION share/${PROJECT_NAME}
)

foreach(_file IN LISTS TRANSLATION_QM_FILES)
    get_filename_component(_filename "${_file}" NAME)
    string(REPLACE "${SHORT_NAME}" "${PROJECT_NAME}" _full_filename "${_filename}")
    install(FILES ${_file} DESTINATION share/${PROJECT_NAME}/translations RENAME ${_full_filename})
endforeach()

set(ICON_SIZES 86x86 108x108 128x128 172x172)
foreach(_size IN LISTS ICON_SIZES)
    install(FILES icons/${_size}/${PROJECT_NAME}.png DESTINATION share/icons/hicolor/${_size}/apps)
endforeach()

install(FILES ${PROJECT_NAME}.desktop
    DESTINATION share/applications
)

file(WRITE "${CMAKE_BINARY_DIR}/QtCreatorDeployment.txt"
    "${CMAKE_INSTALL_PREFIX}
${CMAKE_BINARY_DIR}/${PROJECT_NAME}:bin
")
