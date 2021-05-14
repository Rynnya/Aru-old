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

if (MSVC)
  add_compile_options(
    $<$<CONFIG:>:/MD>
    $<$<CONFIG:Debug>:/MDd>
    $<$<CONFIG:Release>:/MD>
  )
endif()
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
	src/controller/MainController.cpp
	src/controller/MainController.hpp
	src/controller/UsersController.cpp
	src/controller/UsersController.hpp
	src/database/connection/HimitsuDB.cpp
	src/database/connection/HimitsuDB.h
	src/database/connection/Redis.cpp
	src/database/connection/Redis.h
	src/database/tables/CoreTable.h
	src/database/tables/BeatmapTable.h
	src/database/tables/OtherTable.h
	src/database/tables/ScoresTable.h
	src/database/tables/UsersTable.h
	src/database/SQL.h
	src/dto/DTOs.hpp
	src/utils/curl.h
	src/utils/math.h
	src/utils/osu.h
	src/utils/time.h
	src/utils/utils.h
	src/AppComponent.hpp
	src/Config.h
	src/Globals.h
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
