run: compilar
	./bash

compilar: 
	gcc -o bash main.c

clean:
	rm bash

all: clean compilar run