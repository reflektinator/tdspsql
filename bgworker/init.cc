#include "../tdspsql.h"
#if 0
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
}
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include "../tds/tdsmain.h"
#endif

extern "C" {

static void tdsworker(Datum arg);
static void listener(Datum arg);

PG_MODULE_MAGIC;
}

struct ipc_data {
  sockaddr_in tds_sa;
  /* and anything else the bgworker needs to pass to the dynamic worker */
};

static void
tdsworker(Datum arg) {
  int ipc_sock;
  struct sockaddr_un ipc_sa;
  int tds_sock;
  char *logmsg;
  int res;
  BackgroundWorkerUnblockSignals();
  elog(WARNING, "Worker started");
  ipc_sock = socket(AF_UNIX, SOCK_STREAM, 0);
  asprintf(&logmsg, "ipc_sock = %d", ipc_sock);
  elog(WARNING, logmsg);
  free(logmsg);
  ipc_sa.sun_family = AF_UNIX;
  strncpy(ipc_sa.sun_path, "/tmp/tdspsql", sizeof(ipc_sa.sun_path));
  res = connect(ipc_sock, (struct sockaddr *)&ipc_sa, sizeof(ipc_sa));
  asprintf(&logmsg, "worker connect ipc res = %d", res);
  elog(WARNING, logmsg);
  free(logmsg);
  struct msghdr msg;
  struct iovec iov;
  char control_data[CMSG_SPACE(sizeof(tds_sock))];
  struct cmsghdr *cmsg;
  ipc_data data;
  iov.iov_base = &data;
  iov.iov_len = sizeof(data);;
  msg.msg_name = NULL;
  msg.msg_namelen = 0;
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;
  msg.msg_control = control_data;
  msg.msg_controllen = CMSG_SPACE(sizeof(tds_sock));
  res = recvmsg(ipc_sock, &msg, 0);
  asprintf(&logmsg, "bytes_received = %d", res);
  elog(WARNING, logmsg);
  free(logmsg);
  cmsg = CMSG_FIRSTHDR(&msg);
  tds_sock = *((int *)CMSG_DATA(cmsg));
  close(ipc_sock);
  asprintf(&logmsg, "tds_sock = %d", tds_sock);
  elog(WARNING, logmsg);
  
  tdsmain *main = new tdsmain(tds_sock, data.tds_sa);
  main->run();
  delete main;
  
  proc_exit(1);
}

static void
listener(Datum arg) {
  int tds_sock;
  struct sockaddr_in tds_sa;
  int res;
  int ipc_sock;
  struct sockaddr_un ipc_sa;

  BackgroundWorkerUnblockSignals();

  /* IPC socket is for passing socket FD's between processes */
  unlink("/tmp/tdspsql");
  ipc_sock = socket(AF_UNIX, SOCK_STREAM, 0);
  ipc_sa.sun_family = AF_UNIX;
  strncpy(ipc_sa.sun_path, "/tmp/tdspsql", sizeof(ipc_sa.sun_path));
elog(WARNING, ipc_sa.sun_path);
  while ((res = bind(ipc_sock, (struct sockaddr *)&ipc_sa, sizeof(ipc_sa)))) {
    elog(WARNING, "Failed to bind to ipc socket");
    sleep(10);
  }
  elog(WARNING, "IPC socket bound successfully ipc_sock = %d", ipc_sock);
  res = listen(ipc_sock, 1);
  elog(WARNING, "ipc_sock listen res = %d", res);

  tds_sock = socket(AF_INET, SOCK_STREAM, 0);
  tds_sa.sin_family = AF_INET;
  tds_sa.sin_port = htons(1433);
  tds_sa.sin_addr.s_addr = htonl(0);

  while ((res = bind(tds_sock, (struct sockaddr *)&tds_sa, sizeof(tds_sa))) != 0) {
    elog(WARNING, "Failed to bind to tds socket");
    sleep(10);
  }
  elog(WARNING, "TDS socket bound successfully");

  res = listen(tds_sock, 16);

  for (;;) {
    struct sockaddr_in tds_peer_sa;
    socklen_t tds_peer_sa_len;
    int tds_peer_sock;
    struct sockaddr_un ipc_peer_sa;
    socklen_t ipc_peer_sa_len;
    int ipc_peer_sock;
    BackgroundWorker worker;
    BackgroundWorkerHandle *handle;

    tds_peer_sa_len = sizeof(tds_peer_sa);
    tds_peer_sock = accept(tds_sock, (struct sockaddr *)&tds_peer_sa, &tds_peer_sa_len);
    elog(WARNING, "Accepted connection %d", tds_peer_sock);
    strncpy(worker.bgw_name, "tds handler", sizeof(worker.bgw_name));
    worker.bgw_flags = BGWORKER_SHMEM_ACCESS | BGWORKER_BACKEND_DATABASE_CONNECTION;
    worker.bgw_start_time = BgWorkerStart_RecoveryFinished;
    worker.bgw_restart_time = BGW_NEVER_RESTART;
    //worker.bgw_main_arg = 1433;
    worker.bgw_main = tdsworker;
    //worker.bgw_notify_pid = MyProcPid;
    worker.bgw_notify_pid = 0;
    RegisterDynamicBackgroundWorker(&worker, &handle);
    ipc_peer_sa_len = sizeof(ipc_peer_sa);
    ipc_peer_sock = accept(ipc_sock, (struct sockaddr *)&ipc_peer_sa, &ipc_peer_sa_len);
    elog(WARNING, "accept ipc_sock = %d, errno = %s", ipc_peer_sock, strerror(errno));
    struct msghdr msg;
    struct iovec iov;
    char control_data[CMSG_SPACE(sizeof(tds_peer_sock))];
    struct cmsghdr *cmsg;
    char data[1]; /* must have some actual data */
    iov.iov_base = data;
    iov.iov_len = 1;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = control_data;
    msg.msg_controllen = CMSG_SPACE(sizeof(tds_peer_sock));
    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_len = CMSG_LEN(sizeof(tds_peer_sock));
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    *((int *)CMSG_DATA(cmsg)) = tds_peer_sock;
elog(WARNING, "sending message ipc_peer_sock = %d, tds_peer_sock = %d", ipc_peer_sock, tds_peer_sock);
    res = sendmsg(ipc_peer_sock, &msg, 0);
elog(WARNING, "message sent res = %d, errno = %s", res, strerror(errno));
    //wait for the other end to close?
  }
  proc_exit(0);
}

extern "C" {

void
_PG_init(void) {
  BackgroundWorker worker;

//pprintf(WARNING, "*** INIT CALLED ***");
  /* probably read config file for SQL instances to start... */
  strncpy(worker.bgw_name, "tdspsql listener", sizeof(worker.bgw_name));
  worker.bgw_flags = BGWORKER_SHMEM_ACCESS | BGWORKER_BACKEND_DATABASE_CONNECTION;
  worker.bgw_start_time = BgWorkerStart_RecoveryFinished;
  worker.bgw_restart_time = BGW_NEVER_RESTART;
  worker.bgw_notify_pid = 0;
  worker.bgw_main_arg = 1433;
  worker.bgw_main = listener;
  RegisterBackgroundWorker(&worker);

  /* and maybe another worker for sql browser... one day */
}

}
