//engine.c
#include "stdio.h"
#include "header.h"
#include <stdlib.h>


#define FEN6 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"
#define MATE "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -"

int main()
{ 

  Allinit();
  
  S_BOARD board[1];
  initpvtable(board->pvtable);
  S_MOVELIST list[1];
  S_SEARCHINFO info[1];
  parse_fen(FEN6,board);
  //perfttest(5,board);

  char input[6];
  int move = NOMOVE;
  int pvnum = 0;
  int max = 0;
  while(TRUE)
  {
    printboard(board);
    printf("please enter a move > ");
    fgets(input,6,stdin);

    if(input[0]=='q')
      break;
    else if(input[0]=='t')
      takemove(board);
    else if(input[0]=='s')
    {
      info->depth=6;
      searchposition(board,info);
    }
    else
    {
      move = parsemove(input,board);
      if(move != NOMOVE)
      { 
        storepvmove(board,move);
        makemove(board,move);
        /*if(isrepetition(board))
          printf("Repetition occured");*/
      }
      else
        printf("Move not passed: %s\n",input);
    }
    fflush(stdin);
  }
  free(board->pvtable->ptable);
  return 0;
}

