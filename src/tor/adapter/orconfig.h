#ifndef GENERIC_ORCONFIG_H_
#define GENERIC_ORCONFIG_H_

/** String describing which Tor Git repository version the source was
 * built from.  This string is generated by a bit of shell kludging in
 * src/or/include.am, and is usually right.
 */

/*
const char tor_git_revision[] =
#ifndef _MSC_VER
#include "micro-revision.i"
#endif
  "";
 */


#if defined(_WIN32)
#  include "orconfig_win32.h"
#elif defined(__darwin__) || defined(__APPLE__)
#  include "orconfig_apple.h"
#else
#  include "orconfig_linux.h"
#endif

#endif  // GENERIC_ORCONFIG_H_
