bash:
	gcc -o bash main.c

run: bash
	./bash

clean:
	rm bash