#ifndef __crypt_h__
#define __crypt_h__
#include <string>

struct ssl
{
	static void sslInitDictionary();
	static int sslEncrypt( unsigned char* key, const unsigned char* in_data, int in_len, unsigned char* out_data );
	static int sslDecrypt( unsigned char* key, const unsigned char* in_data, int in_len, unsigned char* out_data );
	
	static unsigned int sslCrc32(const char* data, unsigned int len);
	static bool sslChecksum(const char* data, unsigned int len, unsigned int key);

	static std::string ssl_md5(const char* in, size_t len);

protected:
	static unsigned int crc32(unsigned long crc , const char* data, unsigned int len);
};

#endif