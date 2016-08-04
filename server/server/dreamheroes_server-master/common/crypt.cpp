#include <time.h>
#include <stdlib.h>
#include "crypt.h"
#include "md5.h"
namespace Crypto
{
	const unsigned int TMP_BUFFERSIZE = 1024*100;
	const unsigned long crc_table[256] =
	{
		0x00000000UL, 0x77073096UL, 0xee0e612cUL, 0x990951baUL, 0x076dc419UL,
		0x706af48fUL, 0xe963a535UL, 0x9e6495a3UL, 0x0edb8832UL, 0x79dcb8a4UL,
		0xe0d5e91eUL, 0x97d2d988UL, 0x09b64c2bUL, 0x7eb17cbdUL, 0xe7b82d07UL,
		0x90bf1d91UL, 0x1db71064UL, 0x6ab020f2UL, 0xf3b97148UL, 0x84be41deUL,
		0x1adad47dUL, 0x6ddde4ebUL, 0xf4d4b551UL, 0x83d385c7UL, 0x136c9856UL,
		0x646ba8c0UL, 0xfd62f97aUL, 0x8a65c9ecUL, 0x14015c4fUL, 0x63066cd9UL,
		0xfa0f3d63UL, 0x8d080df5UL, 0x3b6e20c8UL, 0x4c69105eUL, 0xd56041e4UL,
		0xa2677172UL, 0x3c03e4d1UL, 0x4b04d447UL, 0xd20d85fdUL, 0xa50ab56bUL,
		0x35b5a8faUL, 0x42b2986cUL, 0xdbbbc9d6UL, 0xacbcf940UL, 0x32d86ce3UL,
		0x45df5c75UL, 0xdcd60dcfUL, 0xabd13d59UL, 0x26d930acUL, 0x51de003aUL,
		0xc8d75180UL, 0xbfd06116UL, 0x21b4f4b5UL, 0x56b3c423UL, 0xcfba9599UL,
		0xb8bda50fUL, 0x2802b89eUL, 0x5f058808UL, 0xc60cd9b2UL, 0xb10be924UL,
		0x2f6f7c87UL, 0x58684c11UL, 0xc1611dabUL, 0xb6662d3dUL, 0x76dc4190UL,
		0x01db7106UL, 0x98d220bcUL, 0xefd5102aUL, 0x71b18589UL, 0x06b6b51fUL,
		0x9fbfe4a5UL, 0xe8b8d433UL, 0x7807c9a2UL, 0x0f00f934UL, 0x9609a88eUL,
		0xe10e9818UL, 0x7f6a0dbbUL, 0x086d3d2dUL, 0x91646c97UL, 0xe6635c01UL,
		0x6b6b51f4UL, 0x1c6c6162UL, 0x856530d8UL, 0xf262004eUL, 0x6c0695edUL,
		0x1b01a57bUL, 0x8208f4c1UL, 0xf50fc457UL, 0x65b0d9c6UL, 0x12b7e950UL,
		0x8bbeb8eaUL, 0xfcb9887cUL, 0x62dd1ddfUL, 0x15da2d49UL, 0x8cd37cf3UL,
		0xfbd44c65UL, 0x4db26158UL, 0x3ab551ceUL, 0xa3bc0074UL, 0xd4bb30e2UL,
		0x4adfa541UL, 0x3dd895d7UL, 0xa4d1c46dUL, 0xd3d6f4fbUL, 0x4369e96aUL,
		0x346ed9fcUL, 0xad678846UL, 0xda60b8d0UL, 0x44042d73UL, 0x33031de5UL,
		0xaa0a4c5fUL, 0xdd0d7cc9UL, 0x5005713cUL, 0x270241aaUL, 0xbe0b1010UL,
		0xc90c2086UL, 0x5768b525UL, 0x206f85b3UL, 0xb966d409UL, 0xce61e49fUL,
		0x5edef90eUL, 0x29d9c998UL, 0xb0d09822UL, 0xc7d7a8b4UL, 0x59b33d17UL,
		0x2eb40d81UL, 0xb7bd5c3bUL, 0xc0ba6cadUL, 0xedb88320UL, 0x9abfb3b6UL,
		0x03b6e20cUL, 0x74b1d29aUL, 0xead54739UL, 0x9dd277afUL, 0x04db2615UL,
		0x73dc1683UL, 0xe3630b12UL, 0x94643b84UL, 0x0d6d6a3eUL, 0x7a6a5aa8UL,
		0xe40ecf0bUL, 0x9309ff9dUL, 0x0a00ae27UL, 0x7d079eb1UL, 0xf00f9344UL,
		0x8708a3d2UL, 0x1e01f268UL, 0x6906c2feUL, 0xf762575dUL, 0x806567cbUL,
		0x196c3671UL, 0x6e6b06e7UL, 0xfed41b76UL, 0x89d32be0UL, 0x10da7a5aUL,
		0x67dd4accUL, 0xf9b9df6fUL, 0x8ebeeff9UL, 0x17b7be43UL, 0x60b08ed5UL,
		0xd6d6a3e8UL, 0xa1d1937eUL, 0x38d8c2c4UL, 0x4fdff252UL, 0xd1bb67f1UL,
		0xa6bc5767UL, 0x3fb506ddUL, 0x48b2364bUL, 0xd80d2bdaUL, 0xaf0a1b4cUL,
		0x36034af6UL, 0x41047a60UL, 0xdf60efc3UL, 0xa867df55UL, 0x316e8eefUL,
		0x4669be79UL, 0xcb61b38cUL, 0xbc66831aUL, 0x256fd2a0UL, 0x5268e236UL,
		0xcc0c7795UL, 0xbb0b4703UL, 0x220216b9UL, 0x5505262fUL, 0xc5ba3bbeUL,
		0xb2bd0b28UL, 0x2bb45a92UL, 0x5cb36a04UL, 0xc2d7ffa7UL, 0xb5d0cf31UL,
		0x2cd99e8bUL, 0x5bdeae1dUL, 0x9b64c2b0UL, 0xec63f226UL, 0x756aa39cUL,
		0x026d930aUL, 0x9c0906a9UL, 0xeb0e363fUL, 0x72076785UL, 0x05005713UL,
		0x95bf4a82UL, 0xe2b87a14UL, 0x7bb12baeUL, 0x0cb61b38UL, 0x92d28e9bUL,
		0xe5d5be0dUL, 0x7cdcefb7UL, 0x0bdbdf21UL, 0x86d3d2d4UL, 0xf1d4e242UL,
		0x68ddb3f8UL, 0x1fda836eUL, 0x81be16cdUL, 0xf6b9265bUL, 0x6fb077e1UL,
		0x18b74777UL, 0x88085ae6UL, 0xff0f6a70UL, 0x66063bcaUL, 0x11010b5cUL,
		0x8f659effUL, 0xf862ae69UL, 0x616bffd3UL, 0x166ccf45UL, 0xa00ae278UL,
		0xd70dd2eeUL, 0x4e048354UL, 0x3903b3c2UL, 0xa7672661UL, 0xd06016f7UL,
		0x4969474dUL, 0x3e6e77dbUL, 0xaed16a4aUL, 0xd9d65adcUL, 0x40df0b66UL,
		0x37d83bf0UL, 0xa9bcae53UL, 0xdebb9ec5UL, 0x47b2cf7fUL, 0x30b5ffe9UL,
		0xbdbdf21cUL, 0xcabac28aUL, 0x53b39330UL, 0x24b4a3a6UL, 0xbad03605UL,
		0xcdd70693UL, 0x54de5729UL, 0x23d967bfUL, 0xb3667a2eUL, 0xc4614ab8UL,
		0x5d681b02UL, 0x2a6f2b94UL, 0xb40bbe37UL, 0xc30c8ea1UL, 0x5a05df1bUL,
		0x2d02ef8dUL
	};
}

unsigned char dictionary[10][256] = 
{
	{0X26, 0Xef, 0Xf7, 0X44, 0Xd6, 0X32, 0Xdf, 0X27, 0Xa0, 0X3c, 0X74, 0Xd0, 0X12, 0Xe3, 0Xe4, 0X3d, 0X2d, 0X9, 0X4d, 0X21, 0X33, 0Xce, 0Xea, 0Xfd, 0Xcc, 0Xe5, 0X6e, 0X4f, 0X31, 0X2, 0Xc6, 0X28, 0Xae, 0X5b, 0Xca, 0X81, 0Xd2, 0Xe, 0X3f, 0X94, 0X38, 0X7d, 0X1, 0X1c, 0Xc8, 0Xf1, 0Xec, 0Xdd, 0X69, 0Xf, 0Xc1, 0X1a, 0Xb6, 0X7e, 0Xb8, 0X78, 0X66, 0Xb2, 0X85, 0X47, 0X10, 0X4a, 0X2e, 0Xd8, 0X30, 0X19, 0X65, 0X84, 0Xe8, 0X24, 0X92, 0X46, 0Xb0, 0X22, 0X54, 0X76, 0X97, 0X9e, 0Xeb, 0X9c, 0X95, 0Xa6, 0Xf3, 0X91, 0X82, 0X7a, 0X34, 0X52, 0X16, 0Xe6, 0X90, 0X7c, 0Xbb, 0Xfb, 0X4, 0Xa2, 0Xe0, 0X18, 0X61, 0Xa8, 0X23, 0X55, 0X48, 0Xb7, 0X58, 0X79, 0X50, 0Xad, 0X56, 0Xc9, 0X8b, 0X1d, 0X11, 0X29, 0Xd7, 0X68, 0X6d, 0X45, 0Xa3, 0Xf2, 0X39, 0Xa9, 0Xda, 0X8e, 0X41, 0X9d, 0X9a, 0X70, 0Xe7, 0X93, 0Xf4, 0Xbd, 0X72, 0Xfa, 0Xc, 0Xa4, 0X7, 0X73, 0Xff, 0Xb4, 0Xde, 0Xf6, 0Xf9, 0X15, 0Xc3, 0X42, 0X75, 0Xc0, 0Xee, 0Xc2, 0Xf0, 0X5a, 0X3b, 0X20, 0X3a, 0X8c, 0X43, 0X2c, 0Xcf, 0X8a, 0Xb9, 0Xb5, 0Xd3, 0Xcb, 0Xaf, 0X80, 0Xf5, 0X5e, 0X2a, 0Xb, 0X35, 0X8f, 0X13, 0Xc5, 0X4e, 0X53, 0X1f, 0X98, 0X51, 0Xe2, 0X60, 0X99, 0Xd1, 0X77, 0X57, 0Xaa, 0X7f, 0X25, 0X63, 0Xbe, 0X9f, 0X40, 0Xd9, 0Xf8, 0X6c, 0X5d, 0Xba, 0X5, 0Xd5, 0X5f, 0Xfc, 0X59, 0Xb1, 0X4c, 0X88, 0X83, 0X62, 0Xac, 0Xa, 0X6f, 0X9b, 0Xdb, 0Xfe, 0X6, 0X37, 0X8d, 0X8, 0X6b, 0X96, 0Xed, 0X89, 0X17, 0Xbc, 0Xe9, 0X14, 0X67, 0X4b, 0X36, 0Xc7, 0X49, 0X5c, 0Xa7, 0Xcd, 0X7b, 0X2f, 0Xdc, 0Xc4, 0Xd4, 0Xa1, 0Xab, 0Xa5, 0Xd, 0X1e, 0Xbf, 0X86, 0Xe1, 0X3e, 0Xb3, 0X3, 0X87, 0X64, 0X71, 0X2b, 0X6a, 0X0, 0X1b},	
	{0X20, 0Xb3, 0Xd, 0Xf5, 0Xc2, 0X9b, 0X18, 0X9f, 0X58, 0X6d, 0Xa2, 0X47, 0Xd5, 0X3d, 0X23, 0X93, 0X8d, 0Xf6, 0Xc7, 0X4d, 0Xe4, 0X10, 0Xc9, 0X71, 0X75, 0X3a, 0X52, 0Xd3, 0Xfb, 0X66, 0Xa9, 0X2c, 0Xf7, 0Xf1, 0X6a, 0X25, 0Xed, 0X6, 0Xb6, 0Xd8, 0X44, 0X4c, 0Xa3, 0X67, 0X39, 0X3f, 0X90, 0Xd0, 0Xd6, 0X32, 0Xe9, 0X68, 0Xb5, 0X7c, 0X96, 0Xcd, 0X5c, 0Xe7, 0X51, 0Xce, 0Xfa, 0Xb9, 0X5a, 0X9c, 0X9e, 0Xa0, 0X4a, 0X24, 0Xaf, 0X95, 0X98, 0X42, 0X53, 0Xec, 0Xd2, 0X6c, 0X35, 0Xf2, 0X99, 0X61, 0Xca, 0X9d, 0X41, 0X37, 0X29, 0Xc0, 0X33, 0Xb8, 0Xba, 0Xa6, 0Xdf, 0Xc6, 0Xb, 0Xe8, 0X60, 0X4f, 0X91, 0X8f, 0X70, 0X22, 0X2e, 0X7e, 0X5e, 0X4e, 0Xbf, 0X7, 0X7d, 0X97, 0Xe3, 0Xa, 0X2d, 0X54, 0X6f, 0X7b, 0X50, 0X3, 0Xa4, 0X62, 0Xf3, 0Xda, 0X2a, 0Xb1, 0Xf8, 0X15, 0Xe2, 0X12, 0X34, 0X85, 0Xe1, 0X76, 0Xf9, 0Xd9, 0X46, 0X8a, 0Xc, 0Xc1, 0Xcc, 0Xbb, 0X1b, 0X49, 0Xab, 0X11, 0Xb7, 0X87, 0X30, 0X4b, 0X77, 0Xf4, 0Xd7, 0Xaa, 0Xc3, 0X55, 0Xef, 0X2, 0Xea, 0Xe5, 0Xfd, 0X16, 0X9, 0Xd4, 0Xa5, 0X28, 0X36, 0X79, 0X1c, 0Xad, 0X2b, 0Xb4, 0X73, 0Xe0, 0Xbd, 0Xbe, 0X1d, 0X64, 0X40, 0X5f, 0X8, 0X1a, 0X17, 0X92, 0X5d, 0X6b, 0Xfc, 0Xde, 0X83, 0X31, 0X57, 0Xf, 0X8c, 0X45, 0Xb2, 0Xc4, 0Xeb, 0X82, 0Xdd, 0X88, 0Xa7, 0X13, 0X1, 0X21, 0X74, 0X2f, 0Xe, 0Xff, 0X27, 0X94, 0X3e, 0Xf0, 0Xdc, 0X78, 0X63, 0X1e, 0Xc8, 0Xcb, 0X1f, 0X43, 0X14, 0X8b, 0Xae, 0Xac, 0X26, 0X59, 0X38, 0X0, 0Xee, 0X8e, 0Xd1, 0X48, 0Xa1, 0Xdb, 0X5b, 0X72, 0Xb0, 0X5, 0Xfe, 0X6e, 0X80, 0X89, 0X65, 0X81, 0X56, 0Xc5, 0X3c, 0X9a, 0X7a, 0X4, 0X84, 0X69, 0X7f, 0Xbc, 0Xcf, 0X86, 0X19, 0Xe6, 0Xa8, 0X3b},
	{0X41, 0Xb9, 0X73, 0Xfa, 0Xf8, 0X6c, 0X4d, 0Xb8, 0X5f, 0X43, 0Xe0, 0X4f, 0Xa3, 0Xbf, 0X16, 0X47, 0X77, 0Xf5, 0X8a, 0Xc8, 0X3b, 0X99, 0X8e, 0X97, 0Xdf, 0Xaa, 0X71, 0X5b, 0Xd9, 0Xc4, 0X2c, 0X5d, 0X44, 0X11, 0X17, 0Xea, 0Xba, 0X28, 0Xf0, 0Xa2, 0X2, 0X0, 0Xe3, 0Xf, 0Xfb, 0Xce, 0X5c, 0Xe5, 0X4b, 0Xc, 0X40, 0X18, 0X82, 0X8d, 0X15, 0X87, 0X24, 0X81, 0X4c, 0X89, 0Xc5, 0X56, 0Xd5, 0X1d, 0X2e, 0X1b, 0X32, 0X6b, 0X66, 0Xfd, 0Xfc, 0X3d, 0X3c, 0X2b, 0Xbb, 0X78, 0Xaf, 0X98, 0Xb3, 0X7d, 0Xb5, 0X1, 0Xae, 0Xb1, 0X53, 0X95, 0X35, 0Xdb, 0X26, 0X4, 0X39, 0X3, 0X4a, 0X90, 0Xad, 0Xb2, 0X9e, 0X8b, 0X14, 0Xee, 0X57, 0Xc2, 0X38, 0Xd, 0X3a, 0X83, 0Xfe, 0X7e, 0X84, 0X54, 0X1e, 0Xe, 0Xcf, 0X70, 0X51, 0X7f, 0Xd6, 0Xb0, 0Xa7, 0X29, 0Xbd, 0X49, 0Xf6, 0X80, 0X93, 0X6f, 0Xc9, 0X9b, 0X92, 0Xec, 0X96, 0Xf2, 0X68, 0X37, 0Xda, 0Xc6, 0X10, 0X59, 0X7, 0Xb, 0X63, 0X76, 0X91, 0X8, 0Xa4, 0Xc0, 0Xdd, 0X5e, 0X33, 0X8c, 0X45, 0Xb7, 0Xcb, 0X60, 0X1c, 0X9d, 0Xe6, 0Xca, 0X42, 0Xed, 0X74, 0X9c, 0X9f, 0Xdc, 0X2d, 0Xf7, 0X85, 0X22, 0Xd8, 0X30, 0X75, 0Xde, 0X6, 0Xd7, 0X9a, 0Xf1, 0Xff, 0X67, 0Xd3, 0X61, 0Xa0, 0X7a, 0Xe1, 0Xd2, 0Xbc, 0Xe9, 0X13, 0X9, 0Xc3, 0Xcd, 0Xef, 0X27, 0X94, 0Xab, 0X62, 0X50, 0Xeb, 0Xbe, 0X12, 0Xe2, 0Xd4, 0Xe7, 0X5a, 0X86, 0X23, 0Xe8, 0Xa8, 0X2f, 0X31, 0Xd1, 0Xe4, 0X19, 0Xc1, 0X88, 0Xa1, 0X25, 0X48, 0Xf9, 0Xb4, 0X20, 0Xf3, 0X3e, 0X34, 0Xa6, 0Xa9, 0X8f, 0X7c, 0X79, 0Xb6, 0X6e, 0X1f, 0X4e, 0Xcc, 0X6d, 0X1a, 0X21, 0X65, 0X36, 0X55, 0X64, 0Xf4, 0X7b, 0Xa, 0X72, 0Xac, 0X46, 0X2a, 0X6a, 0Xa5, 0X3f, 0X52, 0Xc7, 0X69, 0Xd0, 0X5, 0X58},
	{0X9d, 0Xa3, 0X24, 0X9c, 0X86, 0Xed, 0X89, 0X31, 0X2b, 0X7, 0Xe9, 0X6f, 0X3f, 0Xb4, 0Xa0, 0X21, 0X28, 0Xfe, 0Xe7, 0X63, 0X20, 0X5b, 0X65, 0X42, 0Xdf, 0Xff, 0X2d, 0Xd7, 0X49, 0Xef, 0X92, 0Xaa, 0X80, 0Xd3, 0X44, 0Xc1, 0X6e, 0Xce, 0X4b, 0Xb7, 0Xc6, 0X66, 0Xfa, 0Xe8, 0Xa4, 0X91, 0X97, 0X96, 0Xb9, 0Xd6, 0X5c, 0X61, 0X9a, 0X62, 0X85, 0Xbe, 0Xac, 0X45, 0X17, 0X1c, 0X27, 0Xf, 0X67, 0Xcf, 0X38, 0X3a, 0Xf1, 0X11, 0X47, 0Xee, 0Xa9, 0X83, 0X9f, 0Xca, 0X6, 0X6d, 0Xe, 0X2f, 0X7a, 0X41, 0X2e, 0X78, 0X7d, 0Xd9, 0X25, 0X34, 0Xb3, 0Xa8, 0Xfc, 0Xb0, 0X4, 0X90, 0X50, 0X79, 0X48, 0Xbd, 0Xc4, 0Xc5, 0X22, 0X98, 0X81, 0X73, 0X29, 0X8a, 0X40, 0Xf4, 0X54, 0X1e, 0Xfb, 0Xf9, 0Xbb, 0Xf0, 0Xcb, 0X2c, 0X57, 0X51, 0Xc3, 0Xd4, 0Xab, 0X1d, 0Xd, 0Xc9, 0X19, 0Xea, 0X72, 0X36, 0Xa5, 0X8b, 0X8, 0X15, 0X12, 0Xeb, 0Xaf, 0Xb8, 0X3, 0Xd2, 0Xa1, 0Xe4, 0X59, 0Xc, 0X70, 0Xa7, 0X39, 0X23, 0Xd1, 0X94, 0X18, 0Xad, 0Xb1, 0Xd8, 0Xfd, 0X46, 0Xf2, 0Xba, 0X53, 0Xd5, 0X10, 0X7c, 0Xc0, 0X95, 0X3d, 0X43, 0X5a, 0X4c, 0X71, 0X55, 0X74, 0Xcc, 0X35, 0X6c, 0X4f, 0X5, 0X58, 0X82, 0X5d, 0X75, 0X9, 0Xe6, 0X1a, 0Xa2, 0X9b, 0Xdb, 0X8e, 0X5f, 0Xe1, 0X69, 0X5e, 0Xb2, 0X77, 0X76, 0X4d, 0X60, 0Xb, 0X6a, 0X64, 0X33, 0X4a, 0X3b, 0X8d, 0X2a, 0X84, 0X7e, 0X0, 0X1, 0X87, 0X56, 0X4e, 0Xc2, 0Xf3, 0X52, 0Xc8, 0X26, 0X2, 0Xdd, 0X68, 0Xb6, 0Xde, 0X37, 0Xec, 0X7b, 0X88, 0X99, 0X3e, 0X3c, 0X32, 0X1b, 0Xf7, 0X6b, 0Xa, 0X8f, 0X9e, 0X14, 0X13, 0Xda, 0X93, 0Xf6, 0X16, 0Xd0, 0Xa6, 0Xe2, 0X30, 0Xe0, 0Xc7, 0Xf5, 0Xe5, 0X7f, 0Xcd, 0Xdc, 0Xbf, 0X8c, 0Xe3, 0Xf8, 0Xb5, 0Xae, 0X1f, 0Xbc},
	{0X7f, 0X1b, 0Xe, 0Xda, 0X21, 0X7d, 0X52, 0Xb2, 0X4a, 0Xd5, 0X8a, 0X4d, 0X37, 0X26, 0X99, 0Xf6, 0X1a, 0Xdd, 0X6e, 0Xf2, 0X8e, 0X3d, 0X91, 0Xab, 0X71, 0Xcf, 0X3e, 0Xd, 0X62, 0X8f, 0X5d, 0X8, 0Xf9, 0Xf1, 0Xef, 0X88, 0Xec, 0Xfe, 0X32, 0Xd0, 0X87, 0X49, 0X53, 0X30, 0Xe7, 0X15, 0X3a, 0Xed, 0X85, 0Xba, 0X66, 0X75, 0X9d, 0Xc1, 0Xc8, 0Xc9, 0Xf8, 0X13, 0X1d, 0X57, 0Xa6, 0Xde, 0X33, 0Xa7, 0Xd2, 0X46, 0X6b, 0Xf5, 0Xff, 0X19, 0X68, 0X9e, 0Xbf, 0X42, 0Xea, 0X1f, 0X9f, 0X2f, 0X40, 0Xfc, 0Xa4, 0X3c, 0X5, 0Xc, 0Xf4, 0Xc2, 0X5b, 0X20, 0X84, 0X4f, 0X2, 0X97, 0Xf7, 0Xa2, 0X82, 0Xbd, 0Xb0, 0X18, 0Xc3, 0X14, 0X6c, 0Xa3, 0Xe5, 0X25, 0X7a, 0Xf3, 0X9a, 0X35, 0Xe6, 0X28, 0Xe1, 0X6a, 0Xd1, 0Xfd, 0X6, 0Xb5, 0Xa, 0X92, 0X5f, 0X1, 0X27, 0X38, 0Xbb, 0Xc6, 0X80, 0Xeb, 0X24, 0Xb8, 0X86, 0Xd8, 0Xd3, 0X44, 0Xee, 0X67, 0X61, 0X90, 0Xaf, 0Xdc, 0X4e, 0Xcd, 0X83, 0Xb1, 0X1e, 0X45, 0X22, 0Xbe, 0X2c, 0X69, 0X2a, 0X23, 0X74, 0X79, 0Xb3, 0Xa9, 0X16, 0X73, 0X48, 0Xe8, 0Xc7, 0Xe9, 0X1c, 0Xd4, 0X76, 0Xe0, 0Xaa, 0Xb4, 0X47, 0Xb9, 0Xb7, 0X7b, 0X5e, 0X98, 0X93, 0X12, 0Xc5, 0Xca, 0X2d, 0X29, 0X96, 0X6f, 0X17, 0X50, 0X34, 0X78, 0X6d, 0X7, 0Xa8, 0X2e, 0Xd7, 0Xe3, 0Xcb, 0Xd9, 0X31, 0X58, 0X72, 0Xe4, 0Xf0, 0X9b, 0X8c, 0X9, 0Xae, 0X55, 0X11, 0X5a, 0X2b, 0X59, 0X8d, 0X43, 0Xa1, 0X3f, 0X63, 0Xc4, 0X81, 0X10, 0X54, 0Xbc, 0X3, 0Xce, 0X65, 0X4c, 0Xa5, 0Xf, 0Xac, 0X51, 0X7e, 0X7c, 0Xb, 0X41, 0Xdb, 0X60, 0X94, 0X64, 0X5c, 0X4b, 0X4, 0X77, 0X8b, 0Xd6, 0Xc0, 0X95, 0X39, 0X56, 0X3b, 0X9c, 0Xfb, 0Xdf, 0X89, 0X0, 0X70, 0Xe2, 0Xfa, 0Xad, 0Xb6, 0Xa0, 0X36, 0Xcc},
	{0Xa4, 0Xc, 0Xe9, 0X67, 0X7c, 0X3f, 0X2a, 0X0, 0Xef, 0X8c, 0X39, 0X1e, 0X61, 0X55, 0X8f, 0Xc6, 0X23, 0X4d, 0X94, 0X3b, 0Xe, 0X29, 0X71, 0X97, 0X73, 0X14, 0X5c, 0Xc7, 0Xd0, 0X34, 0Xb1, 0X18, 0X17, 0Xfb, 0Xcf, 0Xea, 0Xdf, 0X2e, 0Xf7, 0Xba, 0Xd, 0Xbf, 0Xbc, 0X30, 0X45, 0Xc3, 0X9f, 0X8, 0Xb4, 0Xd6, 0X9a, 0X48, 0X11, 0X8e, 0X13, 0X99, 0X49, 0Xac, 0X5f, 0X8d, 0X3c, 0Xbe, 0X36, 0X5a, 0X32, 0X87, 0X5d, 0X6c, 0Xa, 0Xd4, 0Xf4, 0X5, 0Xf, 0Xa5, 0X91, 0X77, 0X9e, 0X15, 0X47, 0X64, 0X24, 0X4c, 0Xe4, 0X76, 0Xde, 0Xff, 0Xe6, 0Xd5, 0Xf0, 0Xaf, 0Xbb, 0X53, 0X83, 0X72, 0Xee, 0X12, 0X4, 0Xdc, 0X78, 0Xa2, 0X16, 0X81, 0X2, 0X6a, 0X68, 0Xf1, 0X9b, 0X37, 0X8a, 0X58, 0Xf9, 0X3e, 0Xfd, 0X35, 0X3, 0X1f, 0X7b, 0X7a, 0X90, 0Xf3, 0Xda, 0X82, 0X60, 0X80, 0Xd1, 0X50, 0Xb8, 0Xe7, 0X56, 0Xb7, 0Xae, 0X4e, 0Xcc, 0X2f, 0X59, 0Xce, 0Xf5, 0Xe1, 0X9d, 0X84, 0Xd2, 0Xdb, 0X2b, 0X93, 0X6, 0X3a, 0X92, 0Xa7, 0X9c, 0X7, 0X44, 0X6b, 0Xbd, 0Xd8, 0Xb6, 0X46, 0X8b, 0Xf2, 0Xa3, 0X74, 0Xad, 0X88, 0X75, 0X69, 0X79, 0Xd7, 0Xb0, 0Xe2, 0X2c, 0X6f, 0X26, 0Xca, 0X43, 0Xcb, 0Xfa, 0X96, 0X19, 0Xc0, 0X7e, 0X22, 0X86, 0X31, 0Xcd, 0Xa6, 0X62, 0X25, 0X42, 0X3d, 0X5b, 0X1, 0Xc1, 0X57, 0Xec, 0X1b, 0X1c, 0X95, 0Xfe, 0Xab, 0X21, 0X20, 0Xe8, 0Xc8, 0Xeb, 0Xed, 0X1d, 0Xaa, 0Xc5, 0X66, 0X5e, 0Xdd, 0X98, 0Xb, 0X85, 0Xfc, 0X41, 0X4f, 0Xc2, 0X6d, 0X4b, 0Xb3, 0X27, 0Xa1, 0Xf6, 0X28, 0X54, 0X40, 0X10, 0X4a, 0Xa8, 0Xc4, 0Xe5, 0Xb5, 0Xc9, 0Xf8, 0X33, 0X70, 0Xa9, 0X51, 0X65, 0Xd3, 0X6e, 0Xb9, 0X89, 0X7d, 0X9, 0X2d, 0X52, 0X38, 0Xb2, 0X63, 0X7f, 0Xd9, 0Xa0, 0Xe3, 0Xe0, 0X1a},
	{0X12, 0X22, 0X8e, 0Xe6, 0X88, 0Xc2, 0X99, 0Xdd, 0X5d, 0X49, 0X60, 0X6c, 0X2d, 0Xf4, 0X85, 0X8b, 0X67, 0Xc6, 0X5b, 0X1c, 0Xb2, 0Xdb, 0X38, 0Xf8, 0Xbb, 0Xd4, 0Xc4, 0Xae, 0X2a, 0Xb5, 0X76, 0X80, 0Xc9, 0X43, 0Xe4, 0Xcc, 0Xc3, 0X3, 0X20, 0X5, 0X6f, 0X84, 0X94, 0Xce, 0Xd9, 0Xbe, 0Xb8, 0Xb6, 0X7d, 0X54, 0Xf5, 0X13, 0X2, 0Xdf, 0Xfd, 0X86, 0Xa, 0X93, 0Xee, 0X73, 0X9b, 0X68, 0X7a, 0Xbd, 0Xff, 0X31, 0X45, 0X92, 0Xa0, 0Xad, 0X9, 0Xf, 0X3d, 0X1d, 0Xea, 0X7, 0X97, 0Xda, 0X8d, 0X62, 0X56, 0X53, 0Xde, 0X91, 0Xe, 0Xba, 0Xd8, 0X51, 0Xd2, 0X4b, 0Xe1, 0X5a, 0Xa9, 0X4e, 0X70, 0X15, 0X35, 0Xb3, 0X75, 0X36, 0Xac, 0Xe3, 0X7f, 0Xb1, 0X1b, 0Xf7, 0Xf9, 0X98, 0Xd7, 0Xec, 0X7b, 0X64, 0Xd0, 0Xd3, 0X1, 0X11, 0Xc1, 0X1a, 0X3f, 0Xcd, 0X48, 0X21, 0X0, 0Xf0, 0X74, 0X2f, 0Xa2, 0X55, 0Xed, 0X78, 0Xfa, 0X81, 0X42, 0X17, 0X37, 0Xf3, 0Xf6, 0Xe8, 0X29, 0Xb, 0X50, 0X90, 0X3c, 0Xfb, 0X16, 0X27, 0X3b, 0X59, 0Xa1, 0X14, 0X96, 0X82, 0X71, 0X41, 0X9a, 0Xbf, 0X2b, 0X9d, 0X79, 0Xe9, 0Xa7, 0Xcb, 0X89, 0X30, 0Xca, 0Xab, 0X8c, 0Xc, 0Xef, 0X9e, 0X5f, 0X57, 0Xd1, 0X69, 0X58, 0X18, 0X6d, 0X3a, 0X7c, 0X6, 0Xc0, 0Xc8, 0Xf1, 0X63, 0Xa6, 0Xa4, 0X9f, 0X4d, 0X8a, 0X24, 0Xbc, 0Xe0, 0Xe7, 0X32, 0Xaf, 0Xd5, 0X1f, 0X19, 0X8, 0X26, 0Xb4, 0X34, 0X66, 0Xb0, 0X95, 0X2e, 0X7e, 0X5c, 0Xd, 0X28, 0Xa3, 0X61, 0X4c, 0Xfc, 0Xd6, 0X6a, 0X72, 0X33, 0Xaa, 0Xb7, 0X10, 0Xe5, 0Xc5, 0X46, 0X25, 0X4, 0X4a, 0X44, 0X23, 0X47, 0Xf2, 0Xb9, 0Xfe, 0X39, 0X40, 0X52, 0X1e, 0X4f, 0X77, 0Xc7, 0X5e, 0Xeb, 0Xa8, 0X87, 0Xdc, 0X3e, 0X8f, 0X9c, 0X83, 0X2c, 0X65, 0Xe2, 0X6e, 0X6b, 0Xcf, 0Xa5},
	{0X72, 0Xdd, 0X80, 0Xb8, 0X33, 0X3d, 0Xdf, 0X2b, 0Xac, 0Xd0, 0X6, 0X1, 0X9a, 0X4e, 0X4c, 0Xde, 0Xf1, 0Xe1, 0X20, 0X94, 0Xf9, 0Xe6, 0X8e, 0Xfd, 0X14, 0Xf6, 0Xaa, 0X82, 0X15, 0Xce, 0X27, 0X49, 0X6a, 0X96, 0X8c, 0X1f, 0Xb1, 0Xd3, 0Xe0, 0Xa9, 0Xc3, 0X51, 0Xa8, 0Xeb, 0X23, 0X0, 0X4, 0X85, 0Xae, 0X95, 0Xd7, 0X58, 0X83, 0X7d, 0X25, 0Xe3, 0X76, 0Xb2, 0Xc6, 0Xe7, 0Xa, 0Xbc, 0X2f, 0X67, 0X73, 0Xad, 0X90, 0Xdb, 0X5b, 0X8a, 0Xf8, 0X62, 0X21, 0Xf0, 0X6d, 0Xcb, 0X84, 0Xb4, 0X11, 0X98, 0Xaf, 0Xb9, 0Xab, 0X56, 0Xa3, 0Xb0, 0Xbb, 0X50, 0Xf3, 0X8b, 0X5d, 0X5, 0X7e, 0X30, 0Xf4, 0X91, 0Xb7, 0X9e, 0X2, 0Xea, 0X9f, 0X7, 0X5a, 0X77, 0Xc5, 0Xc2, 0X99, 0Xfe, 0X55, 0Xa2, 0X26, 0Xd8, 0X79, 0X8f, 0Xd2, 0Xe4, 0Xed, 0Xff, 0X6e, 0X53, 0X64, 0Xd, 0X45, 0Xc7, 0Xb, 0X3f, 0X6c, 0Xa1, 0Xc4, 0X32, 0Xe, 0X7c, 0Xf5, 0X48, 0X9, 0X34, 0X66, 0X59, 0Xb6, 0X1c, 0X13, 0X42, 0Xda, 0Xd9, 0Xbe, 0X38, 0X37, 0Xec, 0X9d, 0Xef, 0Xba, 0Xe9, 0X31, 0X3e, 0X43, 0X47, 0X28, 0X70, 0Xc9, 0X52, 0Xb3, 0X7a, 0X71, 0Xa7, 0X1e, 0X2c, 0X69, 0X19, 0Xfb, 0X41, 0Xa6, 0X4a, 0X7b, 0X75, 0X6f, 0Xe8, 0X22, 0Xf2, 0Xd1, 0Xa4, 0X2e, 0X3b, 0Xfc, 0Xcd, 0Xc0, 0Xc, 0X8d, 0Xa5, 0X92, 0X5e, 0X57, 0Xf, 0X39, 0X60, 0X36, 0Xcf, 0Xdc, 0X88, 0X63, 0Xee, 0X16, 0X78, 0Xca, 0X4f, 0X9c, 0X1d, 0Xc1, 0X2d, 0Xbf, 0X18, 0X93, 0X40, 0X4d, 0X97, 0X8, 0Xa0, 0X4b, 0X6b, 0Xcc, 0X87, 0Xfa, 0X24, 0Xbd, 0X17, 0X86, 0X3c, 0Xe5, 0X12, 0X35, 0X65, 0Xf7, 0X5f, 0X68, 0X29, 0X3a, 0Xe2, 0X9b, 0X5c, 0X10, 0X44, 0X1b, 0Xc8, 0Xb5, 0X46, 0Xd5, 0X61, 0X3, 0Xd6, 0X2a, 0X81, 0X74, 0X1a, 0X54, 0X89, 0X7f, 0Xd4},
	{0X6e, 0Xc3, 0X57, 0X46, 0X84, 0Xed, 0X29, 0X83, 0Xcb, 0Xa3, 0X6a, 0X9d, 0Xc8, 0X7e, 0Xac, 0X33, 0X4, 0Xf6, 0X4b, 0X3f, 0Xda, 0X6c, 0Xa1, 0Xd7, 0X2a, 0Xba, 0X9f, 0X73, 0X2, 0X23, 0X3d, 0Xe5, 0Xb4, 0Xb7, 0X37, 0Xf, 0X7c, 0X8a, 0Xe8, 0X49, 0X64, 0Xa0, 0Xaf, 0Xf5, 0X42, 0X85, 0Xf2, 0X32, 0X72, 0X17, 0Xad, 0Xde, 0X7f, 0Xcf, 0X98, 0Xf3, 0X95, 0Xf0, 0Xf7, 0X22, 0X65, 0X1e, 0Xc2, 0X10, 0X28, 0X58, 0Xf9, 0Xe, 0X1c, 0Xaa, 0X7a, 0Xb0, 0X13, 0X78, 0X69, 0Xf4, 0X21, 0X3b, 0X51, 0Xb5, 0X4a, 0X81, 0X76, 0X94, 0Xdb, 0Xd3, 0X2b, 0X80, 0X7b, 0X11, 0Xc6, 0X6f, 0Xd9, 0X41, 0X19, 0Xd5, 0X40, 0X7d, 0X6, 0Xc4, 0X31, 0Xb8, 0Xe1, 0Xa5, 0Xca, 0X59, 0Xe3, 0Xe0, 0X63, 0X9c, 0X14, 0X5c, 0Xff, 0X3, 0X2f, 0Xbd, 0Xc7, 0X27, 0X5f, 0Xe7, 0Xb2, 0X56, 0X4d, 0X35, 0X8, 0X2c, 0X47, 0X8f, 0X5e, 0Xbc, 0X25, 0Xe9, 0Xc, 0Xfd, 0Xef, 0Xb1, 0Xfb, 0Xdf, 0X5, 0Xf8, 0X53, 0X89, 0X97, 0X54, 0Xd1, 0Xe2, 0X1, 0Xc0, 0X7, 0X8b, 0X52, 0X5a, 0Xa2, 0Xdc, 0X9e, 0Xcc, 0X0, 0X44, 0Xab, 0Xd8, 0X77, 0X36, 0Xbb, 0Xd0, 0Xa6, 0Xbe, 0Xe4, 0X3c, 0Xea, 0Xcd, 0X91, 0Xa, 0Xe6, 0X30, 0X4e, 0X74, 0Xd2, 0X62, 0X6d, 0X3e, 0X90, 0X8e, 0Xf1, 0Xd6, 0X3a, 0X48, 0X79, 0Xd, 0X88, 0X70, 0Xb, 0X39, 0X1b, 0Xec, 0X68, 0X5d, 0Xc1, 0X9a, 0Xb3, 0X1d, 0Xeb, 0X75, 0Xc5, 0X99, 0X18, 0X6b, 0X15, 0X20, 0X60, 0X38, 0X12, 0X9b, 0X9, 0X67, 0X55, 0X8c, 0Xb9, 0Xa9, 0X82, 0X43, 0Xa8, 0Xee, 0X93, 0X2d, 0Xfe, 0X92, 0X5b, 0Xae, 0Xd4, 0X1a, 0X66, 0Xa4, 0Xb6, 0X61, 0X50, 0X2e, 0Xfa, 0Xdd, 0X4c, 0X24, 0X4f, 0Xce, 0X96, 0X45, 0X26, 0Xa7, 0Xfc, 0Xc9, 0X34, 0Xbf, 0X86, 0X1f, 0X16, 0X8d, 0X87, 0X71},
	{0X66, 0X0, 0X81, 0X79, 0X2e, 0Xc5, 0X6f, 0Xe1, 0X1e, 0Xb4, 0X7d, 0Xff, 0X19, 0Xd0, 0Xcd, 0X8d, 0X2c, 0Xa2, 0X93, 0X24, 0Xfd, 0X4b, 0Xd8, 0X34, 0Xd1, 0Xf, 0Xca, 0X68, 0Xb1, 0Xe0, 0Xba, 0X44, 0X43, 0Xf9, 0X95, 0X91, 0Xc3, 0X31, 0X6c, 0X11, 0X17, 0X32, 0X9c, 0Xb8, 0X16, 0Xbe, 0Xef, 0Xfa, 0X4, 0X8, 0X28, 0Xea, 0X39, 0Xe9, 0X54, 0Xac, 0X71, 0X62, 0Xf2, 0Xd9, 0X36, 0X1b, 0X56, 0X9e, 0Xf5, 0X99, 0Xc2, 0Xd3, 0Xc1, 0X46, 0X26, 0X83, 0Xee, 0X1a, 0X9a, 0X27, 0X4d, 0Xf7, 0X77, 0Xd2, 0X74, 0Xaf, 0Xf1, 0X90, 0X38, 0X63, 0Xd6, 0Xc4, 0X9, 0X1, 0Xfe, 0X58, 0Xe8, 0X84, 0Xbb, 0X73, 0X3a, 0X4f, 0X5d, 0Xc0, 0X3f, 0X5c, 0X75, 0X2, 0X21, 0X61, 0X47, 0X86, 0X88, 0Xdb, 0X13, 0Xcf, 0X6a, 0X40, 0X49, 0X6b, 0X8f, 0X57, 0X3b, 0X18, 0Xfc, 0X9f, 0Xa0, 0X92, 0X8e, 0X69, 0Xd, 0X59, 0Xe4, 0Xf6, 0X97, 0X37, 0X22, 0Xde, 0X20, 0Xd7, 0X76, 0Xe5, 0Xc8, 0Xc7, 0X65, 0X7, 0X5e, 0X3c, 0X35, 0Xdc, 0X6e, 0X5a, 0Xce, 0Xf4, 0X98, 0X45, 0X41, 0Xb5, 0X51, 0X4c, 0X7e, 0X6, 0X23, 0X42, 0X52, 0Xe, 0Xb9, 0Xbc, 0X3d, 0Xcc, 0X2a, 0Xdd, 0X6d, 0X85, 0X29, 0X55, 0Xa4, 0X8a, 0Xb2, 0X30, 0X12, 0X8c, 0Xb, 0X94, 0X8b, 0Xc, 0Xa8, 0X7f, 0Xe7, 0X78, 0X3, 0Xa5, 0X89, 0X4a, 0X3e, 0Xe3, 0Xbf, 0X10, 0Xf3, 0X2f, 0Xb3, 0X72, 0X7a, 0X15, 0Xfb, 0X5, 0Xb7, 0X5f, 0Xe2, 0X50, 0X53, 0X70, 0Xc9, 0Xed, 0X5b, 0Xa9, 0Xa, 0Xb0, 0X7c, 0X67, 0Xf8, 0X14, 0X2b, 0X1d, 0X80, 0X1c, 0Xab, 0Xa7, 0Xb6, 0Xd5, 0Xec, 0X64, 0Xda, 0X96, 0X48, 0Xdf, 0Xa1, 0Xae, 0Xa3, 0Xbd, 0Xaa, 0Xd4, 0X2d, 0X25, 0X9d, 0Xa6, 0Xcb, 0Xeb, 0X1f, 0Xe6, 0X82, 0Xf0, 0X33, 0X7b, 0X9b, 0X60, 0Xad, 0Xc6, 0X4e, 0X87}
};

static unsigned char reversed_dictionary[10][256];
void ssl::sslInitDictionary()
{
	int i, j, k;
	for( i = 0; i < 10; ++i )
	{	
		for( j = 0; j < 256; ++j )
		{
			for( k = 0; k < 256; ++k )
			{
				if( dictionary[i][k] == j )
					break;
			}
			reversed_dictionary[i][j] = k;
		}
	}
	srand( (unsigned int)time( NULL ) );
}
int ssl::sslEncrypt( unsigned char* key, const unsigned char* in_data, int in_len, unsigned char* out_data )
{
	if (*key == 0x00)
	{	return in_len ;}

	int key2 = rand() % 256;
	int i = 0;

	for( ; i < in_len; ++i )
		out_data[i] = dictionary[((unsigned int)key2 + i) % 10][in_data[i]] ^ *key;
	out_data[in_len] = key2;

	//++*key;
	return in_len + 1;
}
int ssl::sslDecrypt( unsigned char* key, const unsigned char* in_data, int in_len, unsigned char* out_data )
{
	if (*key == 0x00)
	{	return in_len  ;}

	int key2 = in_data[in_len - 1];
	int i = 0;

	for( ; i < in_len; ++i )
		out_data[i] = reversed_dictionary[((unsigned int)key2 + i) % 10][in_data[i] ^ *key];

	//++*key;
	return in_len - 1;
}

#define DO1 crc = Crypto::crc_table[((int)crc ^ (*data++)) & 0xff] ^ (crc >> 8)
#define DO8 DO1; DO1; DO1; DO1; DO1; DO1; DO1; DO1

 unsigned int ssl::sslCrc32(const char* data, unsigned int len)
 {
	 unsigned long crc = 0;
	 return ssl::crc32(crc, data, len);
	
 }
 bool ssl::sslChecksum(const char* data, unsigned int len, unsigned int key)
 {
	 return key == ssl::sslCrc32(data, len);
 }
 unsigned int ssl::crc32(unsigned long crc, const char* data, unsigned int len)
 {
	 if (data == NULL)
	 {
		 return 0UL;
	 }

	 crc = crc ^ 0xffffffffUL;
	 while (len >= 8) {
		 DO8;
		 len -= 8;
	 }
	 if (len) do {
		 DO1;
	 } while (--len);
	 return crc ^ 0xffffffffUL;
 }
 std::string ssl::ssl_md5(const char* in, size_t len)
 {
	 Crypto::md5 md5s(in, len);
	 return md5s.tostring();
 }
