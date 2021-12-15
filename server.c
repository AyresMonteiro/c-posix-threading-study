#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#define LIMIT_OF_CLIENTS 2

int server_socket;
int * client_sockets;

int catch_signal(int signal, void (*handler) (int)) {
	struct sigaction action;

	action.sa_handler = handler;

	sigemptyset(&action.sa_mask);

	action.sa_flags = 0;

	return sigaction(signal, &action, NULL);
}

void close_socket_descriptors (int * descriptors, int limit) {
	for (int i = 0; i < limit; i++) {
		if(descriptors[i]) close(descriptors[i]);
	}
}

void log_error(char * message) {	
	fprintf(stderr, "%s: %s\n", message, strerror(errno));
}

void error (char * message) {
	log_error(message);

	if(server_socket) close(server_socket);

	close_socket_descriptors(client_sockets, LIMIT_OF_CLIENTS);

	exit(1);
}

void handle_shutdown(int signal) {
	error("\nSomeone hitted Ctrl + C");
}

int create_server_socket() {
	// About PF_INET:
	// https://stackoverflow.com/questions/6729366/what-is-the-difference-between-af-inet-and-pf-inet-in-socket-programming
	int listener_descriptor = socket(PF_INET, SOCK_STREAM, 0);

	if (listener_descriptor == -1) error("Can't open socket");

	return listener_descriptor;
}

void bind_socket_to_port(int listener_descriptor, int port) {
	// About sockaddr_in:
	// https://www.gta.ufrj.br/ensino/eel878/sockets/sockaddr_inman.html
	struct sockaddr_in name;

	name.sin_family = PF_INET;

	// About in_port_t:
	// https://pubs.opengroup.org/onlinepubs/7908799/xns/arpainet.h.html
	//
	// About htons() and htonl():
	// https://www.gta.ufrj.br/ensino/eel878/sockets/htonsman.html
	//
	// INADDR_ANY specifies that any address will be okay to you.
	name.sin_port = (in_port_t) htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);

	// Mark port of socket as reusable.
	// SOL_SOCKET refers to the socket passed to setsocketopt().
	int canReuse = 1;

	if(setsockopt(listener_descriptor, SOL_SOCKET, SO_REUSEADDR, &canReuse, sizeof(int)) == -1) {
		error("Can't mark port as reusable");
	}

	// Binds socket to a port
	int c = bind(listener_descriptor, (struct sockaddr *) &name, sizeof(name));

	if(c == -1) error("Can't bind socket");	
}

void start_socket_listening(int listener_descriptor, int max_listeners) {
	// Starts server listening
	if(listen(listener_descriptor, 10) == -1) error("Can't listen");
}

int create_client_socket(int listener_descriptor) {
	struct sockaddr_storage client_addr;

	unsigned int address_size = sizeof(client_addr);

	// Accept a connection if is possible
	int connection_descriptor = accept(listener_descriptor, (struct sockaddr *) &client_addr, &address_size);
	
	if (connection_descriptor == -1) error("Can't open connection socket");

	return connection_descriptor;
}

int * generate_socket_descriptors_array (int limit) {
	int * descriptors = (int *) malloc(sizeof(int) * limit);

	for (int i = 0; i < limit; i++) {
		descriptors[i] = 0;
	}

	return descriptors;
}

int send_message_to_client(int listener_descriptor, int connection_descriptor, char * message) {
	int was_message_sent = send(connection_descriptor, message, strlen(message), 0);
	
	if(was_message_sent == -1) log_error("Can't send message");

	return was_message_sent;
}

int read_from_client(int connection_descriptor, char * buffer, int length_of_buffer) {
	char * string = buffer;
	int string_length = length_of_buffer;

	int characters_received = recv(connection_descriptor, string, string_length, 0);

	while (characters_received > 0 && string[characters_received - 1] != '\n') {
		string += characters_received;
		string_length -= characters_received;
		characters_received = recv(connection_descriptor, string, string_length, 0);
	}

	if (characters_received < 0) return characters_received;
	else if (characters_received == 0) buffer[0] = '\0';
	else string[characters_received - 1] = '\0';

	return length_of_buffer - string_length;
}

int main () {
	if(catch_signal(SIGINT, handle_shutdown) == -1) {
		error("Interruption handler wasn't set");
	}
	
	client_sockets = generate_socket_descriptors_array(LIMIT_OF_CLIENTS);

	server_socket = create_server_socket();	

	bind_socket_to_port(server_socket, 3000);

	start_socket_listening(server_socket, 20);

	puts("The listening has been started.\n");

	char buffer[255];

	while(1) {
		client_sockets[0] = create_client_socket(server_socket);

		// check this line after
		send_message_to_client(
				server_socket,
				client_sockets[0],
				"\r\nSIMPLE SERVER TEST - 1.0\r\n");

		send_message_to_client(
				server_socket,
				client_sockets[0],
				"\r\nThis is another message.\r\n");

		send_message_to_client(
				server_socket,
				client_sockets[0],
				"And this is the final message.\r\n\r\n");

		send_message_to_client(
				server_socket,
				client_sockets[0],
				"Programmed by: @AyresMonteiro\r\n\r\n");
		
		close_socket_descriptors(client_sockets, LIMIT_OF_CLIENTS);
	}

	free(client_sockets);

	close(server_socket);

	return 0;
}

