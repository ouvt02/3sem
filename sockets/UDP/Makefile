all: client server

client: UDP_time_client.cpp
	clang++ -o client UDP_time_client.cpp -Wall -Wextra
	
server: UDP_time_server.cpp
	clang++ -o server UDP_time_server.cpp -Wall -Wextra
	
.PHONY: clean
clean: 
	rm client server *~
