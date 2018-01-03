#ifndef _TDSPSQL_H
#define _TDSPSQL_H

#define TDSVERSION_SERVER 0x730B0003 /* TDS 7.3.B 2008R2 */
//#define TDSVERSION_SERVER 0x71000001 /* TDS 7.1   2000 */

class tsql_var;
class tsql_types;
class tsql_results;

extern tsql_types types;

inline static void
pprintf(int level, const char *format, ...);

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <iconv.h>

#include <string>
#include <vector>
#include <map>

using namespace std;
extern "C" {
#include <postgres.h>
#include <postmaster/bgworker.h>
#include <catalog/pg_type.h>
#include <utils/array.h>
#include <utils/builtins.h>
#include <storage/ipc.h>
#include <storage/latch.h>
#include <storage/proc.h>
#include <fmgr.h>
#include <executor/spi.h>
#include <access/xact.h>
#include <access/htup_details.h>
#include <utils/snapmgr.h>
#include <commands/trigger.h>

}

#include "tds/tds.h"
#include "language/language.h"

inline static void
pprintf(int level, const char *format, ...) {
  va_list args;
  char *msg;
  va_start(args, format);
  vasprintf(&msg, format, args);
  va_end(args);
  elog(level, msg);
}

#endif

