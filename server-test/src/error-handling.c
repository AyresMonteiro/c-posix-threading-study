#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int catch_signal(int signal, void (*handler)(int))
{
	struct sigaction action;

	action.sa_handler = handler;

	sigemptyset(&action.sa_mask);

	action.sa_flags = 0;

	return sigaction(signal, &action, NULL);
}

void log_error(char *message)
{
	fprintf(stderr, "%s: %s\n", message, strerror(errno));
}

void error(char *message, void (*handler)())
{
	log_error(message);

	handler();

	exit(1);
}