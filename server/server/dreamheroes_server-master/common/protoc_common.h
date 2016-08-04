#ifndef __protoc_common_h__
#define __protoc_common_h__
#include <google/protobuf/message.h>
#include "thread_buffer.h"
#include "my_log.h"

#define PROTOCO_NAME(CLS) CLS::default_instance().GetTypeName()

typedef unsigned int pb_flag_type;
class ProtocBufferCommon
{
public:
    typedef unsigned int pb_name_type;
   
    static bool initProtocBuffer()
    {
        GOOGLE_PROTOBUF_VERIFY_VERSION;
        return true;
    }
    static void shutDownProtocBuffer()
    {
        google::protobuf::ShutdownProtobufLibrary();
    }

    template<class T>
    static bool pushProtocMessage(google::protobuf::Message* msg, pb_flag_type flag, T* p, void(T::*methond)(const void*data, const unsigned int len))
    {
       if (NULL == msg || NULL == p || NULL == methond)
       {    return false ;}

       const std::string& name = msg->GetTypeName();
       pb_name_type name_len = name.length();
       unsigned int max_len = 0;
       max_len += sizeof(pb_name_type);
       max_len += name_len;
       max_len += msg->ByteSize();

       thread_send_buffer packet(max_len);
       packet.write((char*)&flag, sizeof(pb_flag_type));
       packet.write((char*)&name_len, sizeof(pb_name_type));
       packet.write(name.c_str(), name_len);
	   char temp[524288];

	   msg->SerializeToArray(temp, 524288);
       if (! msg->SerializeToArray(packet.getBegin(),  packet.getLastLen()))
       {    return false ;}
      
       packet.addWriteLen(msg->ByteSize());
       (p->*methond)( packet.getBuff(), packet.getLen());
       return true;
    }
    template<class T>
    static bool parseProtocmessage(const char* buff, const unsigned int len, T* p, void(T::*methond)(google::protobuf::Message*, pb_flag_type))
    {
        if (NULL == buff || 0 == len || NULL == p || NULL == methond)
        {   return false ;}

        thread_recv_buffer packet(buff, len);
        pb_name_type name_len = 0;
        pb_flag_type flag = 0;
        packet.read((char*)(&flag), sizeof(pb_flag_type));
        packet.read((char*)(&name_len), sizeof(pb_name_type));
        if (name_len > packet.getLen() || 0 == name_len )
        {   return false;}

        std::string name(packet.getBegin(), name_len);
        packet.addReadLen(name_len);
		//Mylog::log_database( LOG_INFO, "Protocmessage %s", name.c_str());
        google::protobuf::Message* message = NULL;
        const google::protobuf::Descriptor* descriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(name);
        if (descriptor)
        {
            const google::protobuf::Message* prototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
            if (prototype)
            {   
                message = prototype->New();
                if (!message->ParseFromArray(packet.getBegin(), packet.getLastLen()))
                {
                    delete message ;
                    return false;
                }
                packet.resetReadPos();
            }
        }

        if (NULL == message)
        {   
			 Mylog::log_server(LOG_ERROR,"not found protocol message [%s]", name.c_str());
		     return false;
		}

        (p->*methond)(message, flag);
        delete message;
        return true;
    }
protected:
private:
};

// class ProtocSessionBase
// {
// public:
//     bool sendPBMessage(google::protobuf::Message* msg)
//     {
//         return ProtocBufferCommon::pushProtocMessage< ProtocSessionBase>(msg, this, &ProtocSessionBase::sendPBBuffer);
//     }
// 
//     bool parsePBMessage(const char* buff, const unsigned long len)
//     {
//         return ProtocBufferCommon::parseProtocmessage< ProtocSessionBase >(buff, len, this,&ProtocSessionBase::callPBCBFun);
//     }
// 
//     virtual void callPBCBFun(google::protobuf::Message* msg) = 0;           //呼叫回调函数
//     virtual void sendPBBuffer(const void*data, const unsigned int len) = 0;       //发送消息
// protected:
//    
// private:
//};
template<class T>
class ProtocMsgBase
{
public:
    typedef void(T::*pb_send_fun)(const void*, const unsigned int);
    typedef void(T::*pb_callback_fun)(google::protobuf::Message*, pb_flag_type);
   
    ProtocMsgBase():_proto_user_ptr(NULL)
    {

    }
    ProtocMsgBase( T* o):_proto_user_ptr(o)
    {

    }
    bool sendPBMessage(google::protobuf::Message* msg, pb_flag_type flag = 0)
    {
        return ProtocBufferCommon::pushProtocMessage< ProtocMsgBase<T> >(msg, flag, this, &ProtocMsgBase<T>::sendPBBuffer);
    }

    bool parsePBMessage(const char* buff, const unsigned long len)
    {
        return ProtocBufferCommon::parseProtocmessage< ProtocMsgBase<T> >(buff, len,  this, &ProtocMsgBase<T>::callPBCBFun);
    }

    //find callback function from register list.
    void callPBCBFun(google::protobuf::Message* msg, pb_flag_type flag) 
    {
        if (NULL == _proto_user_ptr)
        {   
			return ;
		}
        typename std::map<std::string, pb_callback_fun >::iterator it;  
        it = static_call_back_funs.find(msg->GetTypeName());
        if (it != ProtocMsgBase<T>::static_call_back_funs.end())
        {
           pb_callback_fun fun = boost::ref( it->second);
           if ( NULL != fun )
           {
               (_proto_user_ptr->*fun)(msg, flag);
               return ;
           }
        }

        if (NULL != static_default_pb_function)
        {
            (_proto_user_ptr->*static_default_pb_function)(msg, flag);
        }
    }
    void sendPBBuffer(const void*data, const unsigned int len)
    {
        if (NULL !=  _proto_user_ptr && NULL != ProtocMsgBase<T>::static_send_function)
        {   (_proto_user_ptr->*static_send_function)( data, len);}
    }

    // register the send message interface and default callback function 
    // PB_MSG_SD_FUN -- T::FUN(char*, const unsigned int); 
    // PB_MSG_CB_FUN -- T::FUN(google::protobuf::Message*);
    static void registerSDFun(pb_send_fun fun, pb_callback_fun defFun)
    {
        ProtocMsgBase<T>::static_send_function = fun;
        ProtocMsgBase<T>::static_default_pb_function = defFun;
    }
    // register the protocol message's callback function
    // str is the protocol message's name .
    // fun is callback function
    // T::FUN(google::protobuf::Message*);
    static void registerCBFun(std::string str, pb_callback_fun fun)
    {
        ProtocMsgBase<T>::static_call_back_funs.insert(std::make_pair(str, fun));
    }

   
protected:
    T* _proto_user_ptr;
private:
    static pb_send_fun static_send_function;                                // class T interface, use it send message. 
    static pb_callback_fun static_default_pb_function;                      // class T interface, this callback function call when not found the  protocol message's callback function 
    static std::map<std::string, pb_callback_fun > static_call_back_funs;   // class T interface, this is used by found the protocol message's callback function. 
    
};
template<class T> 
typename ProtocMsgBase<T>::pb_send_fun ProtocMsgBase<T>::static_send_function = NULL;
template<class T> 
typename ProtocMsgBase<T>::pb_callback_fun ProtocMsgBase<T>::static_default_pb_function = NULL;
template<class T> 
std::map<std::string, typename ProtocMsgBase<T>::pb_callback_fun > ProtocMsgBase<T>::static_call_back_funs ;
#endif
