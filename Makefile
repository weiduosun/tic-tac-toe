all: run_docker

run: rest_server
	LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib ./rest_server

rest_server: rest_server.c
	gcc -o rest_server rest_server.c -lulfius -ljansson

call_server:
	curl -XGET localhost:8080/order

copy_needed_libs:
	mkdir -p libs
	cp /usr/local/lib/libjansson.so.4 ./libs/
	cp /usr/lib/libulfius.so.2.3 ./libs/
	cp /usr/lib/libulfius.so ./libs/
	cp /usr/local/lib/libmicrohttpd.so.12 ./libs/
	cp /usr/lib/libyder.so.1.2.0 ./libs/
	cp /usr/lib/liborcania.so.1.2 ./libs/

docker_make_server: rest_server copy_needed_libs
	sudo docker build -t ttt-server .

run_docker: rest_server copy_needed_libs
	make docker_make_server
	sudo docker run -p 8080:8080 -it ttt-server:latest

clean:
	sudo docker rmi --force ttt-server
	rm -rf ./rest_server ./libs
