// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料

// Windows 头文件:
#include <windows.h>
#include <shlobj.h>
#include <TlHelp32.h>
#include <shellapi.h>
#pragma comment(lib,"shell32.lib")
#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>
#include <direct.h>
#include <restartmanager.h>
#pragma comment(lib, "rstrtmgr.lib")
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#include <cctype>
#include <comdef.h>
#include <Wbemidl.h>
#pragma comment(lib, "wbemuuid.lib")

// C 运行时头文件
#include <tchar.h>
#include <stdint.h>

// C++ 运行时头文件
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>

#define ENABLE_LIBZIP 0

// 第三方库
#include "miniz.h"

#if ENABLE_LIBZIP
#include <libzip/zip.h>
#ifdef _DEBUG
#pragma comment(lib, "lib/libzip/zipd.lib")
#pragma comment(lib, "lib/zlib/zlibstaticd.lib")
#else
#pragma comment(lib, "lib/libzip/zip.lib")
#pragma comment(lib, "lib/zlib/zlibstatic.lib")
#endif // _DEBUG
#endif // ENABLE_LIBZIP

#define CURL_STATICLIB
#include <curl/curl.h>
#pragma comment(lib, "crypt32.lib")
#ifdef _DEBUG
#pragma comment(lib, "lib/curl/libcurl-d.lib")
#else
#pragma comment(lib, "lib/curl/libcurl.lib")
#endif // _DEBUG

#include "sha256.h"
#define nullptr NULL

// TODO: 在此处引用程序需要的其他头文件
