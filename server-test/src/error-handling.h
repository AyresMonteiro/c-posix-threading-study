int catch_signal(int signal, void (*handler)(int));

void log_error(char *message);

void error(char *message, void (*handler)());