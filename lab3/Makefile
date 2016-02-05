all: server

server: server.cpp
	g++ -o server server.cpp -pthread
clean: 
	rm server
run: 
	./server 3000 10 /opt/bitnami/apache2/htdocs/cs360
