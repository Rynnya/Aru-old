cmake_minimum_required(VERSION 3.2)

set(project_name HimitsuAPI)
set(CMAKE_CXX_STANDARD 17)
project(${project_name})

option(USE_MARIADB "Use MariaDB instead of MySQL" OFF)
set(OATPP_BUILD_TESTS OFF CACHE BOOL "Create test target for oat++")
set(BUILD_TESTING OFF CACHE BOOL "Build tests")
set(ENABLE_TESTS OFF CACHE BOOL "Build unit tests")

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/libs/oatpp/CMakeLists.txt)
  message(FATAL_ERROR "oatpp is missing in folder /libs/oatpp. Have you initialized the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${CMAKE_SOURCE_DIR}/libs/fmt/CMakeLists.txt)
  message(FATAL_ERROR "fmt is missing in folder /libs/fmt. Have you initialized the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${CMAKE_SOURCE_DIR}/libs/sqlpp11/CMakeLists.txt)
  message(FATAL_ERROR "sqlpp11 is missing in folder /libs/sqlpp11. Have you initialized the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${CMAKE_SOURCE_DIR}/libs/sqlpp11-connector-mysql/CMakeLists.txt)
  message(FATAL_ERROR "sqlpp11-connector-mysql is missing in folder /libs/sqlpp11-connector-mysql. Have you initialized the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${CMAKE_SOURCE_DIR}/libs/cpp_redis/CMakeLists.txt)
  message(FATAL_ERROR "cpp_redis is missing in folder /libs/cpp_redis. Have you initialized the submodules / downloaded the extra libraries?")
endif()
if (NOT EXISTS ${CMAKE_SOURCE_DIR}/libs/json/CMakeLists.txt)
  message(FATAL_ERROR "nlohmann-json is missing in folder /libs/json. Have you initialized the submodules / downloaded the extra libraries?")
endif()

add_compile_options(
    $<$<CONFIG:>:/MD>
    $<$<CONFIG:Debug>:/MDd>
    $<$<CONFIG:Release>:/MD>
)

set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(libs/oatpp/)
add_subdirectory(libs/fmt/)
add_subdirectory(libs/sqlpp11)
add_subdirectory(libs/sqlpp11-connector-mysql)
add_subdirectory(libs/cpp_redis)
add_subdirectory(libs/json)

include_directories(src)

add_library(${project_name}-lib
	src/controller/BeatmapController.cpp
	src/controller/BeatmapController.hpp
	src/controller/MainController.hpp
	src/controller/SettingsController.cpp
	src/controller/SettingsController.hpp
	src/controller/UsersController.cpp
	src/controller/UsersController.hpp
	src/database/connection/HimitsuDB.cpp
	src/database/connection/HimitsuDB.hpp
	src/database/connection/Redis.cpp
	src/database/connection/Redis.hpp
	src/database/tables/CoreTable.hpp
	src/database/tables/BeatmapTable.hpp
	src/database/tables/OtherTable.hpp
	src/database/tables/ScoresTable.hpp
	src/database/tables/UsersTable.hpp
	src/database/SQL.hpp
	src/handlers/AuthorizationHandler.hpp
	src/handlers/ErrorHandler.hpp
	src/utils/curl.hpp
	src/utils/math.hpp
	src/utils/osu.hpp
	src/utils/time.hpp
	src/utils/utils.hpp
	src/AppComponent.hpp
	src/Config.hpp
	src/Globals.hpp
)

add_executable(${project_name}-exe src/App.cpp)

target_include_directories(${project_name}-lib PRIVATE
  ${CMAKE_SOURCE_DIR}/libs/oatpp/src
  ${CMAKE_SOURCE_DIR}/libs/fmt/include
  ${CMAKE_SOURCE_DIR}/libs/sqlpp11/include
  ${CMAKE_SOURCE_DIR}/libs/sqlpp11-connector-mysql/include
  ${CMAKE_SOURCE_DIR}/libs/cpp_redis/includes
  ${CMAKE_SOURCE_DIR}/libs/json/include
)
target_include_directories(${project_name}-exe PRIVATE
  ${CMAKE_SOURCE_DIR}/libs/oatpp/src
  ${CMAKE_SOURCE_DIR}/libs/fmt/include
  ${CMAKE_SOURCE_DIR}/libs/sqlpp11/include
  ${CMAKE_SOURCE_DIR}/libs/sqlpp11-connector-mysql/include
  ${CMAKE_SOURCE_DIR}/libs/cpp_redis/includes
  ${CMAKE_SOURCE_DIR}/libs/json/include
)

target_include_directories(${project_name}-lib PUBLIC src)
target_link_libraries(${project_name}-lib PUBLIC
  oatpp
  fmt
  sqlpp-mysql
  cpp_redis
  nlohmann_json
)

find_package(CURL)
if (CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIR})
else (CURL_FOUND)
  message(FATAL_ERROR "Could not find the CURL library and development files.")
endif (CURL_FOUND)

target_link_libraries(${project_name}-exe ${project_name}-lib)
add_dependencies(${project_name}-exe ${project_name}-lib)

set_target_properties(${project_name}-lib ${project_name}-exe PROPERTIES
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF
  CXX_STANDARD_REQUIRED ON
)

if (MSVC)
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  set_target_properties(
    oatpp
    oatpp-test
    fmt
    sqlpp-mysql
    cpp_redis
    tacopie
    PROPERTIES FOLDER libs
  )
  add_definitions(/wd4244 /bigobj)
  # https://stackoverflow.com/a/54531576
  file(GLOB_RECURSE _source_list *.cpp* *.h* *.hpp*)
  foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
  endforeach()
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME}-exe)
endif()
