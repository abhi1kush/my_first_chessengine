all:
	gcc engine.c init.c bitboard.c hashkeys.c board.c data.c attack.c io.c  movegen.c validate.c makemove.c prev_perft.c search.c misc.c pvtable.c evaluate.c -o aout
