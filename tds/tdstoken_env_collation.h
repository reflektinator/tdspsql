#ifndef _TDSTOKEN_ENV_COLLATION_H
#define _TDSTOKEN_ENV_COLLATION_H

#include "../tdspsql.h"

class tdstoken_env_collation : public tdstoken {
private:
protected:

public:
  tdstoken_env_collation();
  size_t writetoken(dynbuffer *buffer, uint32_t tdsversion);
};

#endif

