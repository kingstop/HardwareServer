PROJECT(project_dreamheroes)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8) 
INCLUDE(FindBoost)
IF(LINUX)
    MESSAGE("LINUX")
ELSEIF(WIN32)
    MESSAGE("WIN32")
ELSE()
 MESSAGE("not found")
ENDIF()


INCLUDE_DIRECTORIES( 
    ${PROJECT_SOURCE_DIR}/include 
    ${PROJECT_SOURCE_DIR}/include/linklist
   ./../../../external/boost/boost_1_59_0/
    ./../../../external/mysql++-3.2.2/lib/
    ./../../../external/google/protobuf/src/
    ./../../../external/json/include/
    ./../../../external/log4cplus/log4cplus-1.2.0/include/
    ./../../../external/mysql-5.7.10/build/include/
    ./../../../external/mysql-5.7.10/include/
    ./common/
    /usr/local/mysql/include/
    /usr/include/mysql/
   /usr/include/ 
   /usr/local/include/
) 


LINK_DIRECTORIES( 
    ${PROJECT_SOURCE_DIR}/lib 
    /usr/local/lib 
    /usr/lib 
) 

 
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../bin/server/${PROJECT_NAME}/ ) 
set(CMAKE_EXE_LINKER_FLAGS "-lpthread -lrt")
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../lib) 

add_definitions(-DDEBUG -pthread)
add_definitions("-g")
add_definitions(-DLOG4CXX_STATIC)
ADD_SUBDIRECTORY(common) 
ADD_SUBDIRECTORY(databaseserver)
ADD_SUBDIRECTORY(gameserver)
ADD_SUBDIRECTORY(gateserver)
ADD_SUBDIRECTORY(login)
