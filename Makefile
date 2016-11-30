OBJS	= functions.o structs.o structs_cos.o structs_ma.o structs_ham.o
MAINS	=u_main.o main.o
SOURCE	= u_main.c main.c functions.c structs.c structs_cos.c structs_ma.c structs_ham.c 
HEADER	= struct.h functions.h
OUT	= medoids
CC	= gcc
FLAGS	= -g -c
all:$(OBJS) main.o
	$(CC) $(OBJS) main.o -o $(OUT) -lm
test:$(OBJS) u_main.o
	$(CC) $(OBJS) u_main.o -o utest -lm -lcunit
u_main.o: u_main.c
	$(CC) $(FLAGS) u_main.c

main.o: main.c
	$(CC) $(FLAGS) main.c

functions.o: functions.c
	$(CC) $(FLAGS) functions.c
	
structs.o: structs.c
	$(CC) $(FLAGS) structs.c
	
structs_ham.o: structs_ham.c
	$(CC) $(FLAGS) structs_ham.c

structs_cos.o: structs_cos.c
	$(CC) $(FLAGS) structs_cos.c

structs_ma.o: structs_ma.c
	$(CC) $(FLAGS) structs_ma.c


clean:
	rm -rf $(OUT) 
	rm -rf $(OBJS)
	rm -rf $(MAINS)
	rm -rf utest


count:
	wc $(SOURCE) $(HEADER)
