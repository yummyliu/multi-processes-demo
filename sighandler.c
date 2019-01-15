#include "main.h"

static void sig_parent_handler(int signo);
static void sig_child_handler(int signo);
static void sig_handler(int signo);

static void sig_parent_handler(int signo)
{
	switch (signo) {
		case SIGCHLD:
			printf("caught SIGCHLD: %d\n", signo);
			break;
		case SIGINT:
			printf("caught SIGINT: %d\n", signo);
			for (int i = 0; i < clen; ++i) {
				kill(cs[i].pid,SIGQUIT);
			}
			exit(0);
			break;
		default:
			printf("caught UnKnown: %d\n", signo);
			break;
	}
}
static void sig_child_handler(int signo)
{
	switch (signo) {
		case SIGCHLD:
			printf("caught child SIGCHLD: %d\n", signo);
			exit(0);
			break;
		case SIGINT:
			printf("caught child SIGINT: %d\n", signo);
			exit(0);
			break;
		default:
			printf("caught child UnKnown: %d\n", signo);
			break;
	}
}

static void sig_handler(int signo)
{
	if (ischild) {
		sig_child_handler(signo);
	} else {
		sig_parent_handler(signo);
	}
}

void sig_regist() {
	// signal regist
	struct sigaction sig_ac;
	sig_ac.sa_handler = sig_handler;
	sigemptyset(&sig_ac.sa_mask);
	sig_ac.sa_flags = 0;

	sigaction(SIGCHLD, &sig_ac, NULL);
	sigaction(SIGINT,  &sig_ac, NULL);
	sigaction(SIGHUP,  &sig_ac, NULL);
}
