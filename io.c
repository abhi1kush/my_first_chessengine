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



int parsemove(char *ptrchar, S_BOARD *pos)
{
  if(ptrchar[1] > '8' || ptrchar[1] < '1') return NOMOVE;
  if(ptrchar[3] > '8' || ptrchar[3] < '1') return NOMOVE;
  if(ptrchar[0] > 'h' || ptrchar[0] < 'a') return NOMOVE;
  if(ptrchar[2] > 'h' || ptrchar[2] < 'a') return NOMOVE;

  int from = FR2SQ(ptrchar[0]-'a',ptrchar[1]-'1');
  int to = FR2SQ(ptrchar[2]-'a',ptrchar[3]-'1');
  
  //printf("ptrchar:%s from :%d to:%d \n",ptrchar,from,to);
  
  ASSERT(sqonboard(from)&& sqonboard(to));
  S_MOVELIST list[1];
  
  generateallmoves(pos,list);
  int movenum =0;
  int move =0;
  int prompce = EMPTY;

  for(movenum =0; movenum < list->count; movenum++)
  {
    move = list->moves[movenum].move;
    if(FROMSQ(move)==from && TOSQ(move)==to)
    {
      prompce = PROMOTED(move);
      if(prompce != EMPTY)
      {
        if(isrq(prompce) && !isbq(prompce) && ptrchar[4]=='r')
          return move;
        else if(!isrq(prompce) && isbq(prompce) && ptrchar[4]=='b')
          return move;
        else if(isrq(prompce) && isbq(prompce) && ptrchar[4]=='q')
          return move;
        else if(iskn(prompce) && ptrchar[4]=='n')
          return move;
        continue;
      }
      return move;
    }
   }
   return NOMOVE;
}

