#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

void error(char * message) {
	fprintf(stderr, "%s: %s\n", message, strerror(errno));
	exit(1);
}

pid_t forkAndTreatErrors() {
	// Forks every code below (including outside code) to a new process.
	// Fork returns 0 if the current process is already a child process.
	pid_t pid = fork();

	// If any error has occurred while forking, fork() will return -1
	if(pid == -1) error("Error while forking proccess");

	return pid;
}

int main () {
	// This piece of code is above of fork(), so it isn't forked
	printf("Not forked.\n\n");

	pid_t pid = forkAndTreatErrors();
	int pid_status;

	// If the current process is the CHILD PROCESS, pid will be equal to 0, so
	// just quit it using an exec() function	
	if(pid == 0) {
		if(execl("/usr/bin/python3",
			"/usr/bin/python3",
			"hello_world_across_languages.py",
			NULL) == -1) error("Error while calling first command");
	}

	// The main process will wait for the execution of the child process.
	// If you comment this line and not the other, the second script will run
	// first because you're waiting only the second script. It's like
	// async/await in JavaScript, the difference is that JavaScript uses
	// different events in the same process, and this program uses different
	// process.
	waitpid(pid, &pid_status, 0);
	
	pid = forkAndTreatErrors();

	if (pid == 0) {
		if(execlp("python3",
			"python3",
			"hello_world_across_languages_2.py",
			NULL) == -1) error("Error while calling second command");
	}

	waitpid(pid, &pid_status, 0);

	return 0;
}

