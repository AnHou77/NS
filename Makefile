all: Host view insert 404

CC = gcc
Host: host.c
	$(CC) -o Host host.c

view: view.c
	$(CC) -o view.cgi view.c

insert: insert.c
	$(CC) -o insert.cgi insert.c

404: 404.c
	$(CC) -o 404.cgi 404.c

.PHONY: clean,run
clean:
	rm Host 
test:
	./Host