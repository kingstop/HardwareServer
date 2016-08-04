#include "message_interface.h"
#include "crypt.h"
#include "tcpsession.h"

compress_strategy* message_interface::s_compress_interface = NULL;
void message_interface::messageInit( compress_strategy* cs )
{
	ssl::sslInitDictionary();
	message_interface::s_compress_interface = cs;
}

message_t* message_interface::createMessage(tcp_session* from, message_len len)
{
	message_t* p = new message_t( len, from );
	return p;
}
void message_interface::releaseMessage( message_t* p)
{
	delete p;
}

message_t* message_interface::uncompress(tcp_session* from, char* data, unsigned char* p_recv_key, char* buffptr, message_len& size)
{
	//获取BUFF
	message_len& datalen = *(message_len*)data;

	// len too small
	if (datalen <= MESSAGE_HEAD_MASK_BEGIN)
	{ return NULL; }

	unsigned char* _decrypt_buff = (unsigned char*)(data + MESSAGE_HEAD_MASK_BEGIN);
	message_len   _decrypt_len = datalen - sizeof( message_len );

	//解密
	datalen = sizeof(message_len) + _decrypt_len;//ssl::sslDecrypt( p_recv_key, _decrypt_buff, _decrypt_len, _decrypt_buff);
	// mask
	unsigned short type = *(message_len*)( data + MESSAGE_HEAD_MASK_BEGIN );
	// crc32 验证
	message_crc32 checksum = *(message_crc32*)( data + MESSAGE_HEAD_BASIC_BEGIN );
	
    //暂时不启动CRC32验证
	//if( !ssl::sslChecksum(data + MESSAGE_HEAD_LEN, datalen - MESSAGE_HEAD_LEN, checksum) )
	//{ return NULL;}
  
    
	//是否压缩
	if( ( type & COMPRESS_MASK ) && message_interface::s_compress_interface != NULL)
	{
		//解压缩
		if( message_interface::s_compress_interface->uncompress(buffptr , &size, data + MESSAGE_HEAD_LEN, datalen - MESSAGE_HEAD_LEN ) )
		{
			message_t* msg = createMessage(from, size);
			memcpy( msg->data, buffptr, size );
			return msg;
		}else
		{
			return NULL;
		}
	}
	else
	{
		message_t* msg = createMessage(from, datalen - MESSAGE_HEAD_LEN  );
		memcpy( msg->data, data + MESSAGE_HEAD_LEN, datalen - MESSAGE_HEAD_LEN );
		return msg;
	}
}


message_t* message_interface::compress(tcp_session* from, const char* data, message_len len, char* buffptr, message_len& size, unsigned char* p_send_key)
{
	if (len > MESSAGE_COMPRESS_LEN && message_interface::s_compress_interface != NULL)
	{
		if(message_interface::s_compress_interface->compress(buffptr, &size, data, len) )
		{
			if (size < len)
			{
				return makeMessage(from, buffptr, size, p_send_key, true);
			}
		}
	}
	return makeMessage(from, data, len, p_send_key, false);
}

message_t* message_interface::makeMessage(tcp_session* from, const char* data, message_len len,unsigned char* p_send_key, bool _compress)
{
	message_len lenMax = len + MESSAGE_HEAD_LEN  ;
	message_t* p = createMessage(from, lenMax);
	if (!p)
	{ return NULL;}
	memcpy( p->data + MESSAGE_HEAD_LEN , data, len);

	if (len  > 1)
	{
		*(message_crc32*)(p->data + MESSAGE_HEAD_BASIC_BEGIN) = 0;
		*(message_crc32*)(p->data + MESSAGE_HEAD_BASIC_BEGIN) = ssl::sslCrc32( p->data + MESSAGE_HEAD_LEN, len );  // check code

		if (_compress) // 如果是压缩的数据,加入压缩标示
		{
			*(message_mask*)( p->data + MESSAGE_HEAD_MASK_BEGIN ) |= ::COMPRESS_MASK;
		}
		//加密
		unsigned char* _encrypt_buff = (unsigned char*)(p->data +MESSAGE_HEAD_MASK_BEGIN);
		message_len   _encrypt_len = lenMax - sizeof( message_len);  // -- len

		*(message_len*)p->data =  sizeof( message_len) + _encrypt_len ;//ssl::sslEncrypt( p_send_key, _encrypt_buff, _encrypt_len, _encrypt_buff); // len

	}
	return p;
}