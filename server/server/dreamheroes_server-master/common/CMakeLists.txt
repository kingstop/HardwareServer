
#生成的库文件所在目录  
#SET(GATE_SOURCE_LIST game_client.cpp game_manager.cpp login_client.cpp main.cpp server.cpp user_manager.cpp user_session.cpp user_tcp_server.cpp)


FILE(GLOB SOURCE_1 "./*.cpp") 
FILE(GLOB SOURCE_2 "./message/*.pb.cc") 
ADD_LIBRARY(common STATIC ${SOURCE_1} ${SOURCE_2})
