//io.c

#include "stdio.h"
#include "header.h"

char *prsq(const int sq)
{
  static char sqstr[3];
  
  int file=filesbrd[sq];
  int rank=ranksbrd[sq];
  
  sprintf(sqstr,"%c%C",('a'+file),('1'+rank));


  return sqstr;
}

char *prmove(const int move)
{
  static char mvstr[6];

  int ff = filesbrd[FROMSQ(move)];
  int rf = ranksbrd[FROMSQ(move)];
  int ft = filesbrd[TOSQ(move)];
  int rt = ranksbrd[TOSQ(move)];

  int promoted = PROMOTED(move);

  if(promoted)
  {
    char pchar = 'q';
    if(iskn(promoted))
      pchar='n';
    else if(isrq(promoted) && !isbq(promoted))
      pchar = 'r';
    else if(!isrq(promoted) && isbq(promoted))
      pchar = 'b';
    sprintf(mvstr,"%c%c%c%c%c",('a'+ff),('1'+rf),('a'+ft),('1'+rt),pchar);
  }
  else
    sprintf(mvstr,"%c%c%c%c",('a'+ff),('1'+rf),('a'+ft),('1'+rt));
  
  return mvstr; 
}

void printmovelist(const S_MOVELIST *list)
{
  int i =0;
  int score =0;
  int move =0;
  printf("MoveList:%d\n",list->count);
  
  for(i=0; i < list->count ; i++)
  {
    move = list->moves[i].move;
    score = list->moves[i].score;
    printf("Move:%d > %s (score:%d)\n",i+1,prmove(move),score);
  }
  printf("MoveList Total %d Moves:\n\n",list->count);
}
