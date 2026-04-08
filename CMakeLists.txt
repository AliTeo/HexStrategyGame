cmake_minimum_required(VERSION 3.15)
project(HexagonalStrategyGame VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# External dependencies
find_package(Boost QUIET CONFIG COMPONENTS asio system)
if(NOT Boost_FOUND)
    find_package(Boost REQUIRED)
endif()
set(JSON_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/json/include)

# Fetch SFML 3.0.2
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
)

# Configure SFML options
set(SFML_BUILD_AUDIO TRUE)
set(SFML_BUILD_NETWORK FALSE)
set(BUILD_SHARED_LIBS FALSE)

FetchContent_MakeAvailable(SFML)

# Common library (shared between client and server)
add_library(HexGameCommon STATIC
    common/src/HexCoord.cpp
    common/src/Character.cpp
    common/src/Skill.cpp
    common/src/Action.cpp
    common/src/GameState.cpp
    common/src/Protocol.cpp
)

target_include_directories(HexGameCommon PUBLIC
    ${CMAKE_SOURCE_DIR}/common/include
    ${JSON_INCLUDE_DIR}
)

set(HEXGAME_BOOST_TARGETS)
if(TARGET Boost::asio)
    list(APPEND HEXGAME_BOOST_TARGETS Boost::asio)
elseif(TARGET Boost::headers)
    list(APPEND HEXGAME_BOOST_TARGETS Boost::headers)
elseif(TARGET Boost::boost)
    list(APPEND HEXGAME_BOOST_TARGETS Boost::boost)
endif()

if(TARGET Boost::system)
    list(APPEND HEXGAME_BOOST_TARGETS Boost::system)
endif()

if(HEXGAME_BOOST_TARGETS)
    target_link_libraries(HexGameCommon PUBLIC ${HEXGAME_BOOST_TARGETS})
elseif(Boost_INCLUDE_DIRS)
    target_include_directories(HexGameCommon PUBLIC ${Boost_INCLUDE_DIRS})
endif()

# Server executable
add_executable(HexGameServer
    server/src/main.cpp
    server/src/GameServer.cpp
    server/src/GameSession.cpp
    server/src/MatchManager.cpp
    server/src/TurnResolver.cpp
)

target_include_directories(HexGameServer PRIVATE
    ${CMAKE_SOURCE_DIR}/server/include
)

target_link_libraries(HexGameServer PRIVATE HexGameCommon)

# Linux/Unix specific
if(UNIX)
    target_link_libraries(HexGameServer PRIVATE pthread)
endif()

# SFML graphical client executable
add_executable(HexGameClient
    sfml_main.cpp
    client/src/SFMLGameClient.cpp
    client/src/HexRenderer.cpp
)

target_include_directories(HexGameClient PRIVATE
    ${CMAKE_SOURCE_DIR}/client/include
)

target_link_libraries(HexGameClient PRIVATE 
    HexGameCommon 
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

# Linux/Unix specific
if(UNIX)
    target_link_libraries(HexGameClient PRIVATE pthread)
endif()

# Windows GUI launcher for entering server/port and starting client
if(WIN32)
    add_executable(HexGameLauncher WIN32
        client/src/win_launcher.cpp
    )
    target_compile_definitions(HexGameLauncher PRIVATE UNICODE _UNICODE)
endif()
