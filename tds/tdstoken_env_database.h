#ifndef _TDSTOKEN_ENV_DATABASE_H
#define _TDSTOKEN_ENV_DATABASE_H

#include "../tdspsql.h"

using namespace std;

class tdstoken_env_database : public tdstoken {
private:
  string old_database;
  string new_database;
protected:

public:
  tdstoken_env_database();
  size_t writetoken(dynbuffer *buffer, uint32_t tdsversion);
  void set_old_database(string value) { old_database = value; }
  void set_new_database(string value) { new_database = value; }
};

#endif

