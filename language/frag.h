#ifndef _FRAG_H
#define _FRAG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

using namespace std;

class tsql_frag {
public:
  string frag;
  const char *c_str() { return frag.c_str(); }
  void sprintf(const char *fmt, ...) { char *tmp; va_list va; va_start(va, fmt); vasprintf(&tmp, fmt, va); va_end(va); frag = tmp; free(tmp);}
};

#endif
