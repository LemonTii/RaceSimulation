a3: main.o graphics.o helpers.o runners.o dorcs.o
	gcc -o race main.o graphics.o helpers.o runners.o dorcs.o -lpthread -lncurses

main.o: defs.h main.c
	gcc -c main.c

graphics.o: defs.h graphics.c
	gcc -c graphics.c

helpers.o: defs.h helpers.c
	gcc -c helpers.c

runners.o: defs.h runners.c
	gcc -c runners.c

dorcs.o: defs.h dorcs.c
	gcc -c dorcs.c

clean:
	rm -f *.o race
