
#ifndef MUSQLSERVER_COMMON_H
#define MUSQLSERVER_COMMON_H

// config.h needs to be included 1st
#ifdef HAVE_CONFIG_H
#ifdef PACKAGE
#undef PACKAGE
#endif // PACKAGE
#ifdef PACKAGE_BUGREPORT
#undef PACKAGE_BUGREPORT
#endif // PACKAGE_BUGREPORT
#ifdef PACKAGE_NAME
#undef PACKAGE_NAME
#endif // PACKAGE_NAME
#ifdef PACKAGE_STRING
#undef PACKAGE_STRING
#endif // PACKAGE_STRING
#ifdef PACKAGE_TARNAME
#undef PACKAGE_TARNAME
#endif // PACKAGE_TARNAME
#ifdef PACKAGE_VERSION
#undef PACKAGE_VERSION
#endif // PACKAGE_VERSION
#ifdef VERSION
#undef VERSION
#endif // VERSION
#undef PACKAGE
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef VERSION
#endif // HAVE_CONFIG_H

//#include "Platform/Define.h"

#if COMPILER == COMPILER_MICROSOFT
#  pragma warning(disable:4996)                             // 'function': was declared deprecated
#ifndef __SHOW_STUPID_WARNINGS__
#  pragma warning(disable:4244)                             // 'argument' : conversion from 'type1' to 'type2', possible loss of data
#  pragma warning(disable:4355)                             // 'this' : used in base member initializer list
#endif                                                      // __SHOW_STUPID_WARNINGS__
#endif                                                      // __GNUC__

//#include "Utilities/UnorderedMapSet.h"
#include "Utilities/Timer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <assert.h>
#include "ServerDefines.h"
#include "Logging/Log.h"
#include "ref/WinUtil.h"

#if defined(__sun__)
#include <ieeefp.h> // finite() on Solaris
#endif

#include <set>
#include <list>
#include <string>
#include <map>
#include <queue>
#include <sstream>
#include <algorithm>

#include "Utilities/Errors.h"
#include "LockedQueue/LockedQueue.h"
#include "Threading/Threading.h"

#include <ace/Basic_Types.h>
#include <ace/Guard_T.h>
#include <ace/RW_Thread_Mutex.h>
#include <ace/Thread_Mutex.h>
#include <ace/OS_NS_arpa_inet.h>


#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/asio/deadline_timer.hpp>




#define MAX_NUMBER_MAP 101
#define MAX_SERVER_TYPE 4

// Old ACE versions (pre-ACE-5.5.4) not have this type (add for allow use at Unix side external old ACE versions)
#if PLATFORM != PLATFORM_WINDOWS
#  ifndef ACE_OFF_T
/**
 * @brief
 *
 */
typedef off_t ACE_OFF_T;
#  endif
#endif

#if PLATFORM == PLATFORM_WINDOWS
#  if !defined (FD_SETSIZE)
#    define FD_SETSIZE 4096
#  endif
#  include <ace/config-all.h>
#else
#  include <sys/types.h>
#  include <sys/ioctl.h>
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <unistd.h>
#  include <netdb.h>
#endif

#if COMPILER == COMPILER_MICROSOFT

#  include <float.h>

#  define I32FMT "%08I32X"
#  define I64FMT "%016I64X"
#  define snprintf _snprintf
#  define vsnprintf _vsnprintf
#  define finite(X) _finite(X)

#else

#  define stricmp strcasecmp
#  define strnicmp strncasecmp

#  define I32FMT "%08X"
#  if ACE_SIZEOF_LONG == 8
#    define I64FMT "%016lX"
#  else
#    define I64FMT "%016llX"
#  endif /* ACE_SIZEOF_LONG == 8 */

#endif

#  define UI64FMTD ACE_UINT64_FORMAT_SPECIFIER


#define UI64LIT(N) ACE_UINT64_LITERAL(N)

#define SI64FMTD ACE_INT64_FORMAT_SPECIFIER
#define SI64LIT(N) ACE_INT64_LITERAL(N)

#define SIZEFMTD ACE_SIZE_T_FORMAT_SPECIFIER

/**
 * @brief
 *
 * @param f
 * @return float
 */
inline float finiteAlways(float f) { return finite(f) ? f : 0.0f; }

#define atol(a) strtoul( a, NULL, 10)

#define STRINGIZE(a) #a

// used for creating values for respawn for example
#define MAKE_PAIR64(l, h)  uint64( uint32(l) | ( uint64(h) << 32 ) )
#define PAIR64_HIPART(x)   (uint32)((uint64(x) >> 32) & UI64LIT(0x00000000FFFFFFFF))
#define PAIR64_LOPART(x)   (uint32)(uint64(x)         & UI64LIT(0x00000000FFFFFFFF))

#define MAKE_PAIR32(l, h)  uint32( uint16(l) | ( uint32(h) << 16 ) )
#define PAIR32_HIPART(x)   (uint16)((uint32(x) >> 16) & 0x0000FFFF)
#define PAIR32_LOPART(x)   (uint16)(uint32(x)         & 0x0000FFFF)


#define MAX_ACCOUNT_LEN 10
#define MAX_TYPE_PLAYER 8
#define MAX_NUMBER_MAP 101
#define CHECK_LIMIT(value, limit) (((value)< 0)?FALSE:((value)> limit-1)?FALSE:TRUE)

typedef unsigned char BYTE;
typedef unsigned long DWORD;

/**
 * @brief
 *
 */
enum TimeConstants
{
    MINUTE = 60,
    HOUR   = MINUTE * 60,
    DAY    = HOUR * 24,
    WEEK   = DAY * 7,
    MONTH  = DAY * 30,
    YEAR   = MONTH * 12,
    IN_MILLISECONDS = 1000
};

/**
 * @brief
 *
 */
enum LocaleConstant
{
    LOCALE_enUS = 0,                                        // also enGB
    LOCALE_koKR = 1,
    LOCALE_frFR = 2,
    LOCALE_deDE = 3,
    LOCALE_zhCN = 4,
    LOCALE_zhTW = 5,
    LOCALE_esES = 6,
    LOCALE_esMX = 7
};

#define MAX_LOCALE 8
#define DEFAULT_LOCALE LOCALE_enUS

/**
 * @brief
 *
 * @param name
 * @return LocaleConstant
 */
LocaleConstant GetLocaleByName(const std::string& name);

typedef std::vector<std::string> StringVector;

extern char const* localeNames[MAX_LOCALE]; /**< TODO */

/**
 * @brief
 *
 */
struct LocaleNameStr
{
    char const* name; /**< TODO */
    LocaleConstant locale; /**< TODO */
};

extern LocaleNameStr const fullLocaleNameList[]; /**< used for iterate all names including alternative */

/**
 * @brief operator new[] based version of strdup() function! Release memory by using operator delete[] !
 *
 * @param source
 * @return char
 */
inline char* mangos_strdup(const char* source)
{
    char* dest = new char[strlen(source) + 1];
    strcpy(dest, source);
    return dest;
}

// we always use stdlibc++ std::max/std::min, undefine some not C++ standard defines (Win API and some pother platforms)
#ifdef max
#  undef max
#endif

#ifdef min
#  undef min
#endif

#ifndef M_PI
#  define M_PI          3.14159265358979323846
#endif

#ifndef M_PI_F
#  define M_PI_F        float(M_PI)
#endif

#ifndef countof
#define countof(array) (sizeof(array) / sizeof((array)[0]))
#endif

#endif
