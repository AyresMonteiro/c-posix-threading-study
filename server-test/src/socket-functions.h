void close_socket(int socket);

int create_server_socket(void (*error_function)(char *message));

void bind_socket_to_port(int listener_descriptor, int port, void (*error_function)(char *message));

void start_socket_listening(int listener_descriptor, int max_listeners, void (*error_function)(char *message));

int create_client_socket(int listener_descriptor, void (*error_function)(char *message));

int send_message_to_client(int listener_descriptor, int connection_descriptor, char *message, void (*error_function)(char *message));

int read_from_client(int connection_descriptor, char *buffer, int length_of_buffer);
