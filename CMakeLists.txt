cmake_minimum_required(VERSION 3.10)
set(PROJECT_NAME Pendulum)
project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)

file(GLOB_RECURSE source_files
    "src/*.cpp"
)

set(SOURCES ${source_files})

# Detect and add SFML
find_package(SFML 2 REQUIRED COMPONENTS network audio graphics window system)

add_executable(${PROJECT_NAME} ${WIN32_GUI} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE "src" "lib")
set(SFML_LIBS sfml-system sfml-window sfml-graphics sfml-audio)
target_link_libraries(${PROJECT_NAME} ${SFML_LIBS})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
if (UNIX)
   target_link_libraries(${PROJECT_NAME} pthread)
endif (UNIX)

if(MSVC)
  #target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  #target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Copy res dir to the binary directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

if(MSVC)
   foreach(lib ${SFML_LIBS})
      get_target_property(lib_path ${lib} LOCATION)
      file(COPY ${lib_path} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
   endforeach()
endif(MSVC)
