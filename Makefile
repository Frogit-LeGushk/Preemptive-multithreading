CC=gcc
ASM_CC=fasm

CFLAGS=-c -Wall -Werror

SRC=main.c threads.c timers.c speenlock.c
ASM_SRC=__switch_threads.asm

OBJ=$(SRC:.c=.o) $(ASM_SRC:.asm=.o)
MOVE_ALERTS_TO_TRASH=2>/dev/null

target: $(SRC) $(ASM_SRC)
	$(CC) $(CFLAGS) $(SRC)
	$(ASM_CC) $(ASM_SRC)
	$(CC) $(OBJ) -o exe
	./exe

clean:
	-rm $(OBJ) $(MOVE_ALERTS_TO_TRASH) exe
