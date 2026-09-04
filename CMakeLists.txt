cmake_minimum_required(VERSION 4.3)
project(NMSCoords)

set(CMAKE_CXX_STANDARD 26)

add_subdirectory(external/json)

add_executable(NMSCoords
        src/main.cpp)

target_link_libraries(NMSCoords PRIVATE nlohmann_json::nlohmann_json)