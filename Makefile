# Bondaret Lucas 312CC

build:
	gcc -Wall -Wextra -g -o tema1 tema1.c

run: build
	./tema1

clean:
	rm -f tema1
