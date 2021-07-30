# Aru (osu!) - C++ API

API for use in conjunction with unfinished Yukime site, I will create new repository to site alter some time<br>
Fully done, can be used in production.<br>

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
$ mkdir build
$ cd build
$ cmake ..
```

Also, if you use MariaDB, you need use this:
```
$ cmake .. -DUSE_MARIADB=1
```

# Installation on Windows
Download MariaDB or MySQL C++ connector and cURL.
- https://mariadb.com/downloads/#connectors or https://dev.mysql.com/downloads/connector/cpp/
- http://www.confusedbycode.com/curl/

Setup variables to CMake.
```
-DMYSQL_LIBRARY="path/to/mysql/lib/libmysql.lib" -DMYSQL_INCLUDE_DIR="/path/to/mysql/include/mysql"
```
If you use MariaDB, set this. Second requires if you use MariaDB.
```
-DMYSQL_LIBRARY="path/to/mysql/lib/libmariadb.lib" -DMYSQL_INCLUDE_DIR="/path/to/mysql/include/mysql"
-DUSE_MARIADB=1
```

Create CMake files.
```
mkdir build
cd build
cmake ..
```

# Configuration and compilation
Open Config.h file and set variables as you needed.</br>
When everything is written, compile the program.
```
cmake --build .. --config Release
```
