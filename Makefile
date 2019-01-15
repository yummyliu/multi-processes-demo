all:
	gcc -g client.c child.c channel.c sighandler.c main.c -lusual -L/Users/liuyangming/.locallib/lib -o multiprocessdemo
