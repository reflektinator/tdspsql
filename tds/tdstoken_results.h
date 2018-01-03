#ifndef _TDSTOKEN_RESULTS_H
#define _TDSTOKEN_RESULTS_H

#include "../tdspsql.h"

class tdstoken_results : public tdstoken {
private:
  tsql_results *results;
  tsql_types *types;

protected:

public:
  tdstoken_results(tsql_results *results, tsql_types *types);
  size_t writetoken(dynbuffer *buffer, uint32_t tdsversion);
};

#endif

