/* =============================================================================
#     FileName: main.c
#         Desc:
#       Author: LiuYangming
#        Email: sdwhlym@gmail.com
#     HomePage: http://yummyliu.github.io
#      Version: 0.0.1
#   LastChange: 2018-11-23 17:08:39
#      History:
============================================================================= */
#include "main.h"

child_t cs[MAX_CHILDNUM];
int clen = 0;
int my_pos = -1;
bool ischild=false;
int m_pid=-1;
char* procName;

struct event_base* ebase;
struct event ev_accept;

void usage()
{
	printf("demo -p $port -g\n\t-p $port\n\t-c clear debug msg");
}

void demoinit(int argc, char* argv[]) {
	procName = (argv[0]);
	strncpy(argv[0],"demo-master",strlen(argv[0]));
	int ch;
	while ((ch = getopt(argc, argv, "gp:")) != -1) {
		switch (ch) {
			case 'p':
				listen_port = atoi(optarg);
				break;
			case 'g':
				cf_verbose = 1;
				break;
			case '?':
			default:
				usage();
				exit(0);
		}
	}
}

int main(int argc, char *argv[])
{
	demoinit(argc,argv);

	sig_regist();

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0) {
		log_error("ERROR opening socket");
		exit(1);
	}

	struct sockaddr_in serv_addr;
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(listen_port);
	if (bind(listen_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		log_error("ERROR on binding");
		exit(1);
	}

	listen(listen_fd,5);

	ebase = event_init();
	/*	Pre-2.0 versions of Libevent did not have event_assign() or event_new().
	 *	Instead, you had event_set(), which associated the event with the "current" base.
	 *	If you had more than one base, you needed to remember to call event_base_set()
	 *	afterwards to make sure that the event was associated with the base you actually wanted to use.
	 *	Interface
	 *	void event_set(struct event *event, evutil_socket_t fd, short what,
	 *	        void(*callback)(evutil_socket_t, short, void *), void *arg);
	 *	        int event_base_set(struct event_base *base, struct event *event);
	 *	event_set can set only once
	 */
	event_set(&ev_accept, listen_fd, EV_READ|EV_PERSIST, on_accept, NULL);
	if (event_add(&ev_accept, NULL) < 0) {
		log_error("error add event");
	}

	if (0 < event_dispatch()) {
		log_error("error event_dispatch");
	}
}


