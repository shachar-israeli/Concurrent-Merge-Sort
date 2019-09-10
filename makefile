all: ex3q1 ex3q2s ex3q2c ex3q1.o ex3q2s.o ex3q2c.o

ex3q1: ex3q1.c
	gcc -o ex3q1 ex3q1.c
ex3q2s:ex3q2s.c
	gcc -o ex3q2s ex3q2s.c
ex3q2c:ex3q2c.c
	gcc -o ex3q2c ex3q2c.c

ex3q1.o: ex3q1.c
	gcc -c ex3q1.c
ex3q2s.o: ex3q2s.c
	gcc -c ex3q2s.c
ex3q2c.o: ex3q2c.c
	gcc -c ex3q2c.c
