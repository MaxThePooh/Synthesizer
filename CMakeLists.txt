cmake_minimum_required(VERSION 3.20)
project(Piano)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-v -static)

set(DOWNLOADS "/Users/maxroschupkin/Downloads")

set(SFML_DIR "${DOWNLOADS}/SFML-2.5.1")
set(SFML_INCLUDE_DIR "${SFML_DIR}/include")
set(SFML_LIBRARY_DIR "${SFML_DIR}/lib")

link_directories(${SFML_LIBRARY_DIR})
include_directories(${SFML_INCLUDE_DIR})


find_package(SFML 2.5.1 COMPONENTS system audio window graphics network REQUIRED)



link_libraries (sfml-graphics sfml-window sfml-system sfml-audio)


#set(CMAKE_MACOSX_BUNDLE TRUE)

set(SRC font.otf)

if(CMAKE_MACOSX_BUNDLE)
    file(COPY ${SRC} DESTINATION ./OTO_Gravity.app/Contents/MacOS)
else()
    file(COPY ${SRC} DESTINATION ./)
endif()


add_executable(Piano main.cpp)
