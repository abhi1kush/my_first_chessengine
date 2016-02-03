//uci.c

#include "stdio.h"
#include "header.h"
#include <string.h>
#include <string.h>

#define INPUTBUFFER 400*6

void parsego(char * line, S_SEARCHINFO *info, S_BOARD *pos)
{

}

//position fen
//position startpos
// ... moves e2e4 e7e5

void parseposition(char * linein, S_BOARD *pos)
{
  linein +=9;
  char *ptrchar = linein;

  if(strncmp(linein,"startpos",8)==0)
    parse_fen(START_FEN,pos);
  else
  {
    ptrchar = strstr(linein,"fen");
    if(ptrchar == NULL)
      parse_fen(START_FEN,pos);
    else
    {
      ptrchar+=4;
      parse_fen(ptrchar,pos);
    }
  }

  ptrchar = strstr(linein,"moves");
  int move;


  if(ptrchar != NULL)
  {
    ptrchar += 6;
    while(*ptrchar)
    {
      move = parsemove(ptrchar,pos);
      if(move == NOMOVE) 
        break;
      makemove(pos,move);
      pos->ply =0;
      while(*ptrchar && *ptrchar != ' ')
        ptrchar++;
      ptrchar++;
    }
  }
  printboard(pos);
}

void uci_loop()
{
  setbuf(stdin,NULL);
  setbuf(stdout,NULL);

  char line[INPUTBUFFER];
  printf("id name %s\n",NAME);
  printf("id author Bluefever\n");
  printf("uciok\n");

  S_BOARD pos[1];
  S_SEARCHINFO info[1];
  initpvtable(pos->pvtable);

  while(TRUE)
  {
    memset(&line[0],0,sizeof(line));
    fflush(stdout);
    if(!fgets(line,INPUTBUFFER,stdin))
    continue;

    if(line[0]=='\n')
      continue;

    if(!strncmp(line,"isready",7))
    {
      printf("readyok\n");    
      continue;
    }
    else if(!strncmp(line,"position",8))
      parseposition(line,pos);
    else if(!strncmp(line,"ucinewgame",10))
      parseposition("position startpos\n",pos);
    else if(!strncmp(line,"go",2))
      parsego(line,info,pos);
    else if(!strncmp(line,"quit",4))
    {
      info->quit = TRUE;
      break;
    }
    else if(!strncmp(line,"uci",3))
    {
      printf("id name %s\n",NAME);
      printf("id author Abhi\n");
      printf("uciok\n");
    }
    if(info->quit) break;
  }
  free(pos->pvtable->ptable);
}
