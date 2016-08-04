#PROJECT(project_gate)  

#¹¤³ÌÃû  

add_definitions(-lexpat)

FILE(GLOB SOURCE_1 "./*.cpp") 

ADD_EXECUTABLE(dbserver  ${SOURCE_1})

TARGET_LINK_LIBRARIES(dbserver common.a libprotobuf.a libmysqlpp.so libmysqlclient.so libboost_system.so libboost_thread.so liblog4cplus.so libjsoncpp.a)



