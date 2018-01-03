#ifndef _TDSMAIN_H
#define _TDSMAIN_H

#if 0
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#endif

#include "../tdspsql.h"

class tdsmain {
private:
  pthread_t thread;
  int sock;
  sockaddr_in sockaddr;
  int state;

//  char buffer[65536];

  tdsmain();

public:
  tdsmain(int tds_sock, sockaddr_in tds_sa);
  void run();
};

#endif
