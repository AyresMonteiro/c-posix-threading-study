#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdio.h>

void close_socket(int socket)
{
	close(socket);
}

int create_server_socket(void (*error_function)(char *message))
{
	// About PF_INET:
	// https://stackoverflow.com/questions/6729366/what-is-the-difference-between-af-inet-and-pf-inet-in-socket-programming
	int listener_descriptor = socket(PF_INET, SOCK_STREAM, 0);

	if (listener_descriptor == -1)
	{
		error_function("Can't open socket");
	}

	return listener_descriptor;
}

void bind_socket_to_port(int listener_descriptor, int port, void (*error_function)(char *message))
{
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
	name.sin_port = (in_port_t)htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);

	// Mark port of socket as reusable.
	// SOL_SOCKET refers to the socket passed to setsocketopt().
	int canReuse = 1;

	if (setsockopt(listener_descriptor, SOL_SOCKET, SO_REUSEADDR, &canReuse, sizeof(int)) == -1)
	{
		error_function("Can't mark port as reusable");
	}

	// Binds socket to a port
	int c = bind(listener_descriptor, (struct sockaddr *)&name, sizeof(name));

	if (c == -1)
	{
		error_function("Can't bind socket");
	}
}

void start_socket_listening(int listener_descriptor, int max_listeners, void (*error_function)(char *message))
{
	// Starts server listening
	if (listen(listener_descriptor, 10) == -1)
	{
		error_function("Can't listen");
	}
}

int create_client_socket(int listener_descriptor, void (*error_function)(char *message))
{
	struct sockaddr_storage client_addr;

	unsigned int address_size = sizeof(client_addr);

	// Accept a connection if is possible
	int connection_descriptor = accept(listener_descriptor, (struct sockaddr *)&client_addr, &address_size);

	if (connection_descriptor == -1)
	{
		error_function("Can't open connection socket");
	}

	return connection_descriptor;
}

int send_message_to_client(int listener_descriptor, int connection_descriptor, char *message, void (*error_function)(char *message))
{
	int was_message_sent = send(connection_descriptor, message, strlen(message), 0);

	if (was_message_sent == -1)
	{
		error_function("Can't send message");
	}

	return was_message_sent;
}

int read_from_client(int connection_descriptor, char *buffer, int length_of_buffer)
{
	char *string = buffer;
	int string_length = length_of_buffer;

	int characters_received = recv(connection_descriptor, string, string_length, 0);

	if (characters_received < 0)
	{
		return characters_received;
	}

	while (characters_received > 0 && string_length > 0 && string[characters_received - 1] != '\n')
	{
		string += characters_received;
		string_length -= characters_received;
		characters_received = recv(connection_descriptor, string, string_length, 0);
	}

	if (characters_received == 0)
	{
		buffer[0] = '\0';
	}
	else
	{
		string[characters_received - 2] = '\0';
		string[characters_received - 1] = '\0';
		string_length -= characters_received;
	}

	return length_of_buffer - string_length;
}
