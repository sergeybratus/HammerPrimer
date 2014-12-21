all: base64

base64: base64.c
	gcc -g -o base64 base64.c -lhammer `pkg-config --libs --cflags glib-2.0`

clean:
	rm base64	
