#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "error-handling.h"
#include "socket-functions.h"

int server_socket;

void custom_server_error_handler()
{
	if (server_socket)
	{
		close_socket(server_socket);
	}
}

void server_error(char *message)
{
	error(message, custom_server_error_handler);
}

void handle_shutdown(int signal)
{
	server_error("Someone hitted Ctrl + C!");
}

int main()
{
	if (catch_signal(SIGINT, handle_shutdown) == -1)
	{
		server_error("Interruption handler wasn't set");
	}

	char buffer[255];
	char messageBuffer[500];
	int client_socket = 0;

	server_socket = create_server_socket(server_error);

	bind_socket_to_port(server_socket, 3000, server_error);

	start_socket_listening(server_socket, 20, server_error);

	puts("The listening has been started.\n");

	client_socket = create_client_socket(server_socket, server_error);

	send_message_to_client(server_socket, client_socket, "Hallo... ?\r\n", log_error);
	send_message_to_client(server_socket, client_socket, "What is your name?\r\n", log_error);

	int name_size = read_from_client(client_socket, buffer, 255);

	printf("name_size: %d\n", name_size);

	buffer[name_size > 254 ? 255 : name_size] = '\0';

	sprintf(messageBuffer, "Hallo %s!", buffer);

	send_message_to_client(server_socket, client_socket, messageBuffer, log_error);

	close_socket(client_socket);
	close_socket(server_socket);

	return 0;
}
