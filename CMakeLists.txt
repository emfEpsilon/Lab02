# Minimum required version of CMake
cmake_minimum_required(VERSION 3.18)

# Project metadata
project(
    "Lab01Logistics"
    VERSION 0.1.0
    DESCRIPTION "Lab1 for setting up the development environment for Operating System II"
    LANGUAGES CXX C
)

# Set C and C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)

# Define a build target option (server or client)
option(TARGET "Build target: server or client" server)

# Validate the TARGET option and include common directories and source files
if(${TARGET} STREQUAL "server" OR ${TARGET} STREQUAL "client")
    # Include common headers
    #include_directories(include/common)

    # Collect common source files
    #file(GLOB COMMON_SRC src/common/*.c)find_package

endif()

# Configuration for the server target
if(${TARGET} STREQUAL "server")
    # Include server-specific headers
    include_directories(include/server)

    # Collect server-specific source files
    file(GLOB SERVER_SRC src/server/*.cpp)

    # Find required dependencies for the server
    find_package(mongocxx REQUIRED)
    find_package(httplib REQUIRED)
    find_package(nlohmann_json REQUIRED)

    # Define the server executable and link libraries
    add_executable(server ${SERVER_SRC})

    # Link the server executable with the required libraries
    target_link_libraries(server PRIVATE mongo::mongocxx_static httplib::httplib nlohmann_json::nlohmann_json)

    # Set compiler options for the server
    #target_compile_options(server PRIVATE -Wconversion -O0)
    target_compile_options(server PRIVATE  -Wconversion -std=c++20 -Wno-unused-parameter -O0)
    #target_compile_options(server PRIVATE -Wall -pedantic -Werror -Wextra -Wconversion -std=c++20)

# Configuration for the client target
elseif(${TARGET} STREQUAL "client")
    # Define a preprocessor macro for the client target
    add_definitions(-DTARGET_CLIENT)

    # Include client-specific headers
    include_directories(include/client)

    # Collect client-specific source files
    file(GLOB CLIENT_SRC src/client/*.c)

    # Find required dependencies for the client
    find_package(cJSON REQUIRED)
    find_package(SQLite3 REQUIRED)
    

    # Define the client executable and link libraries
    add_executable(client ${CLIENT_SRC} ${COMMON_SRC})

    # Link the client executable with the required libraries
    target_link_libraries(client PRIVATE cjson::cjson SQLite::SQLite3)

    # Set compiler options for the client
    #arget_compile_options(client PRIVATE -Wall -Wpedantic -Werror -Wextra -Wconversion -std=gnu11)
endif()

# Configuration for the tests and coverage 
if(${TARGET} STREQUAL "tests" OR ${TARGET} STREQUAL "coverage")
    add_subdirectory(tests)
endif()