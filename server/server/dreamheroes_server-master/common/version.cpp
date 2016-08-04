#include "version.h"
#include "stdio.h"

std::string Version::toString()
{
    char buf[256];
    memset(buf, 0 , sizeof(buf));
	sprintf(buf, "%u.%u.%u.%u", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD, VERSION_SVN);
	return std::string(buf);
}
bool Version::checkVersion(const Version& v)
{
	return (nMajor == v.nMajor &&  nMinor == v.nMinor && nBuild == v.nBuild && nAppSvn == v.nAppSvn);
}

bool Version::checkVersion(u16 major, u16 minor, u16 build, u16 svn)
{
	return (nMajor == major && nMinor == minor && nBuild == build && svn == nAppSvn);
}