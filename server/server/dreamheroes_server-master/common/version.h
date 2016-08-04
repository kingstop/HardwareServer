#ifndef __version_h__
#define __version_h__
#include "common_header.h"
#include "common_type.h"
#define PROJECT_NAME	 "FU"
#define VERSION_MAJOR    0      // Application major version number.
#define VERSION_MINOR    0	    // Application minor version number.	
#define VERSION_BUILD    1      // Internal version number. 	
#define VERSION_SVN		 1      // SVN revision number

struct Version
{
	Version()
	{
		nMajor = VERSION_MAJOR;
		nMinor = VERSION_MINOR;
		nBuild = VERSION_BUILD;
		nAppSvn = VERSION_SVN;
	}
	u16 nMajor;
	u16 nMinor;
	u16 nBuild;
	u16 nAppSvn;

	static std::string toString();
	bool checkVersion(const Version& v);
	bool checkVersion(u16 major, u16 minor, u16 build, u16 svn);
};
#endif