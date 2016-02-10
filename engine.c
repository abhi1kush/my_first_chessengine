//engine.c
#include "stdio.h"
#include "header.h"
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

#define FEN6 "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1"
#define MATE "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - -"

int main()
{ 

  Allinit();
  S_BOARD pos[1];
  S_SEARCHINFO info[1];
  info->quit = FALSE;
  info->depth = SETDEPTH;
  pos->hashtable->ptable=NULL;
  inithashtable(pos->hashtable,16);
  setbuf(stdin,NULL);
  setbuf(stdout,NULL);

  printf("Welcome to abhi_engine! type cons for console mode ...\n");
  char line[256];

  while(TRUE)
  {
    memset(line,0,sizeof(line));
    fflush(stdout);
    if(!fgets(line,256,stdin))
      continue;
    if(line[0]=='\n')
      continue;
    if(!strncmp(line,"uci",3))
    {
      uci_loop(pos,info);
      if(info->quit == TRUE)
        break;
      continue;
    }
    else if(!strncmp(line,"xboard",6))
    {
        xboard_loop(pos,info);
        if(info->quit == TRUE)
          break;
        continue;
    }
    else if(!strncmp(line,"cons",4))
    {
      console_loop(pos,info);
      if(info->quit == TRUE)
        break;
      continue;
    }
    else if(!strncmp(line,"quit",4))
      break;
  }
  free(pos->hashtable->ptable);
  
  return 0;
}


























/*
 *
 *
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
      info->starttime = gettime();
      info->stoptime = gettime() + 200000;

      searchposition(board,info);
    }
    else
    {
      move = parsemove(input,board);
      if(move != NOMOVE)
      { 
        storepvmove(board,move);
        makemove(board,move);
        //if(isrepetition(board))
          //printf("Repetition occured");
      }
      else
        printf("Move not passed: %s\n",input);
    }
    fflush(stdin);
  }
  */
