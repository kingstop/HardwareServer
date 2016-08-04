#ifndef __message_interface_h__
#define __message_interface_h__

class tcp_session ;
typedef unsigned int	message_len;    //��������
typedef unsigned short	message_mask;   //����ʶ����
typedef unsigned int	message_crc32;  //����֤�붨��
/************************************************************************/
/* һ����Ϣ��,������ͷ���ֺ����ݲ���,��ͷ����Ϊ����+��ʶ+����֤��       */
/*          MESSAGE_HEAD_MASK_BEGIN�����ͷ��ʶ��ʼλ��.                */
/*          MESSAGE_HEAD_BASIC_BEGIN�����ͷ����֤�뿪ʼλ��.           */
/************************************************************************/

const unsigned int	MESSAGE_HEAD_MASK_BEGIN = sizeof(message_len);
const unsigned int	MESSAGE_HEAD_BASIC_BEGIN = MESSAGE_HEAD_MASK_BEGIN + sizeof(message_mask);
const unsigned int  MESSAGE_HEAD_LEN = sizeof(message_len) + sizeof(message_mask) + sizeof(message_crc32);
const unsigned int	MESSAGE_COMPRESS_LEN = 512;
const unsigned int	COMPRESS_MASK = 1 << 15;

struct message_t
{
	message_t( message_len l, tcp_session* ower):from(ower)
	{
		len = l ;
		data = new char[len ];
	}
	~message_t()
	{
		if (data)
		{
			delete [] data;
		}
	}
	char* data;
	message_len len;
	tcp_session* from;
protected:
	message_t()
	{

	}
	message_t( const message_t&)
	{

	}
};

struct compress_strategy
{
	virtual bool compress( char* dest, unsigned int* destlen, const char* src, unsigned int srclen ) = 0;
	virtual bool uncompress( char* dest, unsigned int* destlen, const char* src, unsigned int srclen ) = 0;
};

struct message_interface
{
	static compress_strategy* s_compress_interface;
	static void messageInit( compress_strategy* cs);

	static message_t* createMessage(tcp_session* from, message_len len);
	static void releaseMessage( message_t* p);
	static message_t* uncompress(tcp_session* from, char* data, unsigned char* p_recv_key, char* buffptr, message_len& size);
	static message_t* compress(tcp_session* from, const char* data, message_len len, char* buffptr, message_len& size, unsigned char* p_send_key);
	static message_t* makeMessage(tcp_session* from, const char* data, message_len len,unsigned char* p_send_key, bool _compress);
};
#endif