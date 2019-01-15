#include "main.h"
int listen_fd;
int listen_port;

struct event ev_channel;

int spawn_worker(int child_accept_sockfd) {

	client_cache = slab_create("client_cache", sizeof(client_t), 0, construct_client, USUAL_ALLOC);
	m_pid = getpid();
	ischild = true;
	my_pos = clen;
	strncpy(procName,"demo-slave",strlen(procName));

	client_t* curcli =  (client_t*)slab_alloc(client_cache);
	if (!curcli) {
		log_warning("cannot allocate client struct");
		close(child_accept_sockfd);
		return -1;
	}

	event_set(&(curcli->ev_cli), child_accept_sockfd, EV_READ|EV_PERSIST, client_handler, curcli);
	if (event_add(&(curcli->ev_cli), NULL) < 0) {
		log_error("error add event");
	}
	event_set(&ev_channel, cs[my_pos].channel[1], EV_WRITE|EV_PERSIST, channel_handler, NULL);
	if (event_add(&ev_channel, NULL) < 0) {
		log_error("error add event");
	}
	return 0;
}

void on_accept(int sockfd, short ev, void *arg){

	log_debug("on_accept: %d, %p\n", ev, arg);

	struct sockaddr_in cli_addr;
	socklen_t clilen = sizeof(cli_addr);

	int child_accept_sockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
	if (child_accept_sockfd < 0) {
		log_error("ERROR on accept");
		exit(1);
	}
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, cs[clen].channel) == -1) {
		log_error("socketpair() failed while spawning \"%s\"", "worker");
	}

	m_pid = fork();
	if (m_pid < 0) {
		log_error("ERROR on fork");
		exit(1);
	} else if (m_pid == 0) {

		spawn_worker(child_accept_sockfd);

		event_del(&ev_accept);
		close(cs[my_pos].channel[0]);
		close(listen_fd);

		log_debug("%s %d\n", "this is child: ", m_pid);
	} else {
		close(cs[clen++].channel[1]);
		close(child_accept_sockfd);

		log_debug("fork one child");
	}
}
