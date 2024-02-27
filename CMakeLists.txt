cmake_minimum_required(VERSION 3.26.1)

project(WaveTrace)

set(USE_GTK_GUI ON)

find_package(PkgConfig REQUIRED)

if (USE_GTK_GUI)
    pkg_check_modules(GTKMM gtkmm-4.0)
    pkg_check_modules(GLIBMM glibmm-2.68)
endif()

pkg_check_modules(PORTAUDIO portaudiocpp)
pkg_check_modules(FFTW3 fftw3)
pkg_check_modules(GLOG libglog)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_compile_definitions(GUI_SCALE=2)

option(USE_GTK_GUI "enable this to enable compiling gtkmm related files; otherwise the gtkmm library will be excluded from compilation." OFF)


if (USE_GTK_GUI)
    add_compile_definitions(USE_GTK)
    include_directories(${GTKMM_INCLUDE_DIRS} ${GLIBMM_INCLUDE_DIRS})
    link_directories(${GTKMM_LIBRARY_DIRS})
endif()

add_definitions(-DNDEBUG) # if this line exists, DLOG calls from glog will not be compiled.
# set(CMAKE_BUILD_TYPE Debug)

include_directories(${PORTAUDIO_INCLUDE_DIRS} ${FFTW3_INCLUDE_DIRS} ${GLOG_INCLUDE_DIRS})
link_directories(${FFTW3_LIBRARY_DIRS} ${GLOG_LIBRARY_DIRS})

file(GLOB_RECURSE SOURCES "./src/*.cpp" "./src/*/*.cpp")

set(MAIN main.cpp)
# set(MAIN "./test/audio-buffer-test.cpp") # Uncomment this and comment the above to test the audio buffer.
# set(MAIN "./test/compass-test.cpp") # Uncomment this and comment the above to test the audio compass.

add_executable(${PROJECT_NAME} ${SOURCES} ${MAIN})

target_include_directories(${PROJECT_NAME} PRIVATE .)

target_link_libraries(${PROJECT_NAME} ${GTKMM_LIBRARIES} ${GLIBMM_LIBRARIES} ${FFTW3_LIBRARIES} ${PORTAUDIO_LIBRARIES} ${GLOG_LIBRARIES} gflags)  # add later: ${GTKMM_LIBRARIES} ${GLIBMM_LIBRARIES} ${PORTAUDIO_LIBRARIES} ${FFTW3_LIBRARIES}




