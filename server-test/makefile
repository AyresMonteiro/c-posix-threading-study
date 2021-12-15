# Build simple server application

server: error-handling.o socket-functions.o
	gcc src/server.c -O3 -o server obj/error-handling.o obj/socket-functions.o

error-handling.o:
	gcc src/error-handling.c -c -O3 -o obj/error-handling.o

socket-functions.o:
	gcc src/socket-functions.c -c -O3 -o obj/socket-functions.o

clean-obj:
	rm obj/*.o
