all: client server


client: client.cpp
	clang++ -o client client.cpp -lrt -Wall -Wextra

	
server: server.cpp
	clang++ -o server server.cpp -lrt -Wall -Wextra
	
PHONY: clean
clean:
	rm client server *~
	
