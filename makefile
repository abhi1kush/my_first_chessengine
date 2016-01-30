all:
	gcc engine_prev.c init.c bitboard.c hashkeys.c board.c data.c attack.c io.c  movegen.c validate.c makemove.c perft_prev.c -o aout
