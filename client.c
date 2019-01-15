#include "main.h"

int cf_maxclients;
struct Slab *client_cache;

void client_handler(int fd, short ev, void *arg)
{
	u_char *buf;
	int len;
	client_t* curcli = (client_t*)arg;
	struct bufferq* bufq;

	switch (ev) {
		case EV_READ:
			if (!(buf = (u_char*)malloc(BUFLEN)))
				log_error("malloc failed");

			len = read(fd, buf, BUFLEN);
			if (len == 0) {
				close(fd);
				event_del(&curcli->ev_cli);

				log_info("Client disconnected.\n");
				return;
			} else if (len < 0) {
				close(fd);
				event_del(&curcli->ev_cli);

				log_debug("Socket failure, disconnecting client: %s",
						strerror(errno));
				return;
			}

			log_debug("%d got msg: %s\n", m_pid, buf);

			break;
		case EV_WRITE:
			bufq = (struct bufferq*)calloc(1, sizeof(struct bufferq));
			if (bufq == NULL)
				err(1, "malloc faild");
			bufq->buf = buf;
			bufq->len = len;
			bufq->offset = 0;
			TAILQ_INSERT_TAIL(&curcli->writeq, bufq, entries);

			log_debug("write");
			break;
		default:
			log_error("unpredicted event %d", ev);
			break;
	}
}

void construct_client(void* obj) {
	client_t *client = (client_t*)obj;

	memset(client, 0, sizeof(client_t));
	client->state = CL_FREE;
}
