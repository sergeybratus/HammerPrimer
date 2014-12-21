all: json 

json: json.c
	gcc -o json json.c -lhammer `pkg-config --libs --cflags glib-2.0`

clean:
	rm json
