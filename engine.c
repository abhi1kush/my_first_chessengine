//engine.c
#include "stdio.h"
#include "header.h"
#include <stdlib.h>


#define FEN6 "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1"


int main()
{ 

  Allinit();
  
  S_BOARD board[1];
  S_MOVELIST list[1];
  
  parse_fen(START_FEN,board);
  //perfttest(5,board);

  char input[6];
  int move = NOMOVE;
  while(TRUE)
  {
    printboard(board);
    printf("please enter a move > ");
    fgets(input,6,stdin);

    if(input[0]=='q')
      break;
    else if(input[0]=='t')
      takemove(board);
    else if(input[0]=='p')
      perfttest(4,board);
    else
    {
      move = parsemove(input,board);
      if(move != NOMOVE)
      {
        makemove(board,move);
        /*if(isrepetition(board))
          printf("Repetition occured");*/
      }
      else
        printf("Move not passed: %s\n",input);
    }
    fflush(stdin);
  }
  
  return 0;
}

