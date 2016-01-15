all:
	gcc engine.c init.c bitboard.c hashkeys.c board.c data.c attack.c io.c  movegen.c validate.c -o aout
