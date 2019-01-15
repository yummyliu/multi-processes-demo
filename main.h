#ifndef GLOBAL_H
#define GLOBAL_H

#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <usual/event.h>
#include <err.h>
#include <sys/socket.h>
#include <sys/queue.h>

#include "usual/logging.h"
#include "usual/slab.h"

#define MAX_CHILDNUM 5
#define BUFLEN 256

#define CL_FREE 0

typedef void (*spawn_proc_pt) (void *data);
typedef struct event my_event_t;
typedef struct child_s child_t;
typedef struct client_s client_t;

struct client_s {
    struct event ev_cli;

	int state;
    /* the queue of data to be written to this client.
     * we can't call write(2) until libevent tells us the socket
     * is ready for writing. */
    TAILQ_HEAD(, bufferq) writeq;
};

struct child_s {
	int		channel[2];
	int		pid;
	int     status;

	spawn_proc_pt   proc;
	void    *data;

	client_t conn_clients;
};

struct bufferq {
    u_char *buf;
    int len;
    int offset;
    /* For the linked list structure. */
    TAILQ_ENTRY(bufferq) entries;
};

typedef struct {
     uint	command;
     pid_t	pid;
     int	slot;
     int	fd;
} my_channel_t;

extern child_t cs[MAX_CHILDNUM];
extern int clen;
extern int my_pos;

void doprocessing (void* data);
void sig_regist();

extern bool ischild;
extern int m_pid;
extern char* procName;
extern int listen_port;
extern int listen_fd;

extern struct event_base* ebase;

extern int cf_verbose; // debug

extern struct event ev_accept;
extern struct event ev_read;
extern struct event ev_channel;

extern struct Slab *client_cache;

void on_accept(int fd, short ev, void *arg);
void client_handler(int fd, short ev, void *arg);
void channel_handler(int fd, short ev, void *arg);

void construct_client(void* obj);
#endif /* ifndef GLOBAL_H */
