CC = gcc
INCLUDE_DIR = ./include
OBJ = sysprak-client
CFLAGS = -g -Wall -Werror

vpath %.c src
vpath %.h include

${OBJ}: main.c performConnection.o connectServer.o prologPhase.o config.o sharedMemory.o gamePhase.o gamelogic.o gameOver.o thinker.o thinkerTool.o
	${CC} ${CFLAGS} -I${INCLUDE_DIR} $^ -o ${OBJ}

%.o: %.c *.h
	$(CC) $(CFLAGS) -I${INCLUDE_DIR} -c $<

.PHONY: clean

clean:
	rm -f *.o ${OBJ}
