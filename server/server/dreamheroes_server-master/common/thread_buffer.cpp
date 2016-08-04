#include "thread_buffer.h"
#include <cstdlib>
#include <cstring>

std::vector<thread_packet_buffer*> thread_buffer_manager::thread_buffer_vector;
boost::mutex thread_buffer_manager::_thread_mutex;