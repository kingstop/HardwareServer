

#¹¤³ÌÃû  

FILE(GLOB SOURCE_1 "./*.cpp") 

ADD_EXECUTABLE(login  ${SOURCE_1})

TARGET_LINK_LIBRARIES(login common.a libprotobuf.a libmysqlpp.so libmysqlclient.so libboost_system.so libboost_thread.so liblog4cplus.so libjsoncpp.a)



