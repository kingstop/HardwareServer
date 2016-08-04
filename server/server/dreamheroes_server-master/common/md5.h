#ifndef __cryto_md5_h__
#define __cryto_md5_h__
#include <iosfwd>
#include <string>

typedef unsigned char byte;
using std::string;
using std::ifstream;

namespace Crypto
{
class md5 {
public:
	md5();
	md5(const void *input, size_t length);
	md5(const string &str);
	md5(ifstream &in);
	void update(const void *input, size_t length);
	void update(const string &str);
	void update(ifstream &in);
	const byte* digest();
	string tostring();
	void reset();
private:
	void update(const byte *input, size_t length);
	void final();
	void transform(const byte block[64]);
	void encode(const unsigned int *input, byte *output, size_t length);
	void decode(const byte *input, unsigned int *output, size_t length);
	string bytesToHexString(const byte *input, size_t length);

	md5(const md5&);
	md5& operator=(const md5&);
private:
	unsigned int _state[4];	/* state (ABCD) */
	unsigned int _count[2];	/* number of bits, modulo 2^64 (low-order word first) */
	byte _buffer[64];	/* input buffer */
	byte _digest[16];	/* message digest */
	bool _finished;		/* calculate finished ? */

	static const byte PADDING[64];	/* padding for calculate */
	static const char HEX[16];
	static const size_t BUFFER_SIZE = 1024;
};
}
#endif
