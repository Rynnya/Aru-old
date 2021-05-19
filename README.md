# Himitsu API (osu!)

API for use in conjunction with [Makatymba2001](https://github.com/makatymba2001) Himitsu [site](https://github.com/makatymba2001/-WIP-himitsu-web)

Still WIP, some paths are not verified.

# Requirements
- C++ compiler
- CMake >= 3.2

# Installation on Linux
First of all, download all libraries.
```
$ apt-get install libmysqld-dev libmysqlclient-dev libcurl-dev
```
If you use MariaDB, then you need download others.
```
$ apt-get install libmariadb-dev libmariadbclient-dev libcurl-dev
```
Afterwards, initialize and download submodules.
```
$ git submodule init && git submodule update --recursive
```
Create CMake files.
```
$ cmake . 
```

If CURL or MYSQL_LIBRARY not found, then set manually.
```
-DCURL_INCLUDE_DIRS="path/to/curl" -DCURL_LIBRARIES="/path/to/curl/libcurl.lib"
-DMYSQL_LIBRARY="path/to/mysql/lib/libmysql.lib" -DMYSQL_INCLUDE_DIR="/path/to/mysql/include"
```
Also, if you use MariaDB, this might be helpful.
```
-DUSE_MARIADB=1
```

# Installation on Windows
Download MariaDB or MySQL C++ connector and cURL.
- https://mariadb.com/downloads/#connectors or https://dev.mysql.com/downloads/connector/cpp/
- http://www.confusedbycode.com/curl/

Setup variables to CMake.
```
-DMYSQL_LIBRARY="path/to/mysql/lib/libmysql.lib" -DMYSQL_INCLUDE_DIR="/path/to/mysql/include/mysql"
```
If you use MariaDB, set this. Second line might be helpful, but not necessary.
```
-DMYSQL_LIBRARY="path/to/mysql/lib/libmariadb.lib" -DMYSQL_INCLUDE_DIR="/path/to/mysql/include/mysql"
-DUSE_MARIADB=1
```

Create CMake files.
```
cmake .
```

# Configuration and compilation
Open Config.h file and set variables as you needed.</br>
When everything is written, compile the program.
```
cmake --build . --config Release
```

# Troubleshooting
In GCC compiler for some reasons <date/date.h> cannot be found in sqlpp11<br>
To fix it, copy include folder from /libs/date/include to /usr/local/include
