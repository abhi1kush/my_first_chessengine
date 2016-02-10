//uci.c

#include "stdio.h"
#include "header.h"
#include <string.h>
#include <string.h>

#define INPUTBUFFER 400*6

// go depth 6 wtime 180000 btime 100000 binc 1000 winc 1000 movetime 1000 movestogo 40
void parsego(char * line, S_SEARCHINFO *info, S_BOARD *pos)
{
  int depth = -1,movestogo =30,movetime = -1;
  int time = -1, inc =0;
  char *ptr = NULL;
  info->timeset = FALSE;

  if((ptr=strstr(line,"infinite")))
  {
    ;
  }

  if((ptr = strstr(line,"binc")) && pos->side == BLACK)
    inc = atoi(ptr + 5);
  
  if((ptr = strstr(line,"winc")) && pos->side == WHITE)
    inc = atoi(ptr + 5);
  
  
  if((ptr = strstr(line,"wtime")) && pos->side ==WHITE)
    time = atoi(ptr+6);

  if((ptr = strstr(line,"btime")) && pos->side == BLACK)
    time = atoi(ptr+6);

  if((ptr = strstr(line,"movestogo")))
    movestogo = atoi(ptr + 10);

  if((ptr = strstr(line,"movetime")))
    movetime = atoi(ptr + 9);

  if((ptr = strstr(line,"depth")))
    depth = atoi(ptr + 6);

  if(movetime != -1)
  {
    time = movetime;
    movestogo = 1;
  }
  info->starttime = gettime();
  info->depth = depth;

  if(time != -1)
  {
    info->timeset = TRUE;
    time /= movestogo;
    time -= 50;
    info->stoptime = info->starttime + time + inc;
  }

  if(depth == -1)
    info->depth = MAXDEPTH;

  printf("time: %d start :%ld stop : %ld depth : %d timeset :%d\n",time,info->starttime,info->stoptime,info->depth,info->timeset);
  searchposition(pos,info);
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

void uci_loop(S_BOARD *pos, S_SEARCHINFO *info)
{
  setbuf(stdin,NULL);
  setbuf(stdout,NULL);

  int MB=16;
  char line[INPUTBUFFER];
  printf("id name %s\n",NAME);
  printf("id author Bluefever\n");
  printf("uciok\n");

  //initpvtable(pos->pvtable);

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
    else if(!strncmp(line,"debug",4))
    {
      DebugAnalysisTest(pos,info);
    }
    else if(!strncmp(line,"setoption name Hash value ",26))
    {
      sscanf(line,"%*s %*s %*s %d",&MB);
      if(MB<4) MB = 4;
      if(MB >2048) MB = 2048;
      printf("Set Hash to %d MB\n",MB);
      inithashtable(pos->hashtable,MB);
    }
    if(info->quit) break;
  }
  //free(pos->pvtable->ptable);
}
