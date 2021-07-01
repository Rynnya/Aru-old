cmake_minimum_required(VERSION 3.2)

set(project_name Aru)
set(CMAKE_CXX_STANDARD 17)
project(${project_name})

option(USE_MARIADB "Use MariaDB instead of MySQL" OFF)
set(OATPP_BUILD_TESTS OFF CACHE BOOL "Create test target for oat++")
set(BUILD_TESTING OFF CACHE BOOL "Build tests")
set(ENABLE_TESTS OFF CACHE BOOL "Build unit tests")
set(OATPP_DISABLE_ENV_OBJECT_COUNTERS ON CACHE BOOL "Do not count oatpp objects")
if (CMAKE_BUILD_TYPE MATCHES DEBUG)
	set(OATPP_DISABLE_ENV_OBJECT_COUNTERS OFF CACHE BOOL "Do not count oatpp objects")
endif()

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
else()
	add_compile_options(-O2 -lcurl -lopenssl)
	if (USE_MARIADB)
		add_compile_options(-lmariadb)
	else()
		add_compile_options(-lmysql)
	endif()
endif()

set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(libs/oatpp/)
add_subdirectory(libs/fmt/)
add_subdirectory(libs/sqlpp11)
add_subdirectory(libs/sqlpp11-connector-mysql)
add_subdirectory(libs/cpp_redis)
add_subdirectory(libs/json)

include_directories(src)

add_executable(${project_name}
	src/controller/AuthController.cpp
	src/controller/AuthController.hpp
	src/controller/BeatmapController.hpp
	src/controller/BeatmapController.cpp
	src/controller/MainController.hpp
	src/controller/SettingsController.cpp
	src/controller/SettingsController.hpp
	src/controller/UsersController.cpp
	src/controller/UsersController.hpp
	src/database/connection/AruDB.cpp
	src/database/connection/AruDB.hpp
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
	src/handlers/HeaderHandler.hpp
	src/handlers/RateLimitHandler.hpp
	src/utils/curl.hpp
	src/utils/hash.hpp
	src/utils/math.hpp
	src/utils/osu.hpp
	src/utils/time.hpp
	src/utils/utils.hpp
	src/AppComponent.hpp
	src/Config.cpp
	src/Config.hpp
	src/Globals.hpp
	src/App.cpp
)

target_include_directories(${project_name} PRIVATE
	${CMAKE_SOURCE_DIR}/libs/oatpp/src
	${CMAKE_SOURCE_DIR}/libs/fmt/include
	${CMAKE_SOURCE_DIR}/libs/sqlpp11/include
	${CMAKE_SOURCE_DIR}/libs/sqlpp11-connector-mysql/include
	${CMAKE_SOURCE_DIR}/libs/cpp_redis/includes
	${CMAKE_SOURCE_DIR}/libs/date/include
)

find_package(CURL)
if (CURL_FOUND)
	include_directories(${CURL_INCLUDE_DIR})
	target_link_libraries(${project_name} PUBLIC ${CURL_LIBRARY})
else (CURL_FOUND)
	message(FATAL_ERROR "Could not find the CURL library and development files.")
endif (CURL_FOUND)

find_package(OpenSSL)
if (OPENSSL_FOUND)
	include_directories(${OPENSSL_INCLUDE_DIR})
	target_link_libraries(${project_name} PUBLIC ${OPENSSL_LIBRARIES})
else (OPENSSL_FOUND)
	message(FATAL_ERROR "Could not find the OpenSSL library and development files.")
endif (OPENSSL_FOUND)

target_include_directories(${project_name} PUBLIC src)
target_link_libraries(${project_name} PUBLIC
	oatpp
	fmt
	sqlpp-mysql
	cpp_redis
	nlohmann_json
)

if (NOT MSVC)
	if (USE_MARIADB)
		target_link_libraries(${project_name} PUBLIC mariadb)
	else()
		target_link_libraries(${project_name} PUBLIC mysql)
	endif()
else()
	target_link_libraries(${project_name} PUBLIC ${MYSQL_LIBRARY})
	include_directories(${MYSQL_INCLUDE_DIR})
endif()

set_target_properties(${project_name} PROPERTIES
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
	file(GLOB_RECURSE _source_list *.c* *.cpp* *.h* *.hpp*)
	foreach(_source IN ITEMS ${_source_list})
		get_filename_component(_source_path "${_source}" PATH)
		string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
		string(REPLACE "/" "\\" _group_path "${_group_path}")
		source_group("${_group_path}" FILES "${_source}")
	endforeach()
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME})
endif()
