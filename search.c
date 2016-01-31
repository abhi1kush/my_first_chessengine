// search.c

#include <stdio.h>
#include "header.h"

#define INFINITE 30000
#define MATE 29000

static void checkup()
{
  //check if time up, or interrupt from GUI
}

static int isrepetition(const S_BOARD *pos)
{
 int i=0;
 for(i=pos->historyply - pos->fiftymove; i<pos->historyply-1 ;++i)
 {
   ASSERT(i>=0 && i<MAXMOVES);
   if(pos->poskey==pos->history[i].poskey)
     return TRUE;
 }
 return FALSE;
}

static void clearforsearch(S_BOARD *pos, S_SEARCHINFO *info)
{
  int i=0;
  int i2=0;

  for(i=0;i<13;i++)
    for(i2=0;i2<SQ_NUM;i2++)
      pos->searchhistory[i][i2] = 0;
  
  for(i=0;i<2;i++)
    for(i2=0;i2<MAXDEPTH;i2++)
      pos->searchkillers[i][i2] = 0;

  clearpvtable(pos->pvtable);
  pos->ply=0;

  info->starttime = gettime();
  info->stopped=0;
  info->nodes=0;
  info->fh =0;
  info->fhf =0;

}


static int alphabeta(int alpha,int beta,int depth, S_BOARD *pos , S_SEARCHINFO *info, int donull)
{
  ASSERT(checkboard(pos));
  if(depth == 0)
  {
    info->nodes++;
    return evalposition(pos);
  }

  info->nodes++;

  if(isrepetition(pos) || pos->fiftymove >= 100)
  {
    return 0;
  }

  if(pos->ply > MAXDEPTH -1)
  {
     return evalposition(pos);
  }

  S_MOVELIST list[1];
  generateallmoves(pos,list);

  int movenum = 0;
  int legal =0;
  int oldalpha = alpha;
  int bestmove = NOMOVE;
  int score = -INFINITE;

  for(movenum=0; movenum < list->count; ++movenum)
  {
    if(!makemove(pos,list->moves[movenum].move))
      continue;
    legal++;
    score = -alphabeta(-beta,-alpha,depth - 1,pos,info,TRUE);
    takemove(pos);
    
    if(score > alpha)
    {
      if(score >= beta)
      {
        if(legal ==1)
          info->fhf++;
        
        info->fh++;
        return beta;
      }
      alpha = score;
      bestmove = list->moves[movenum].move;
    }
  }

  if(legal == 0)
  {
    if(sqattacked(pos->kingsq[pos->side],pos->side^1,pos))
      return -MATE + pos->ply;
    else
      return 0;
  }

  if(alpha != oldalpha )
    storepvmove(pos,bestmove);

  return alpha;
}

static int quiescence(int alpha, int beta, S_BOARD *pos,S_SEARCHINFO *info,int donull)
{
  return 0;
}

void searchposition(S_BOARD *pos, S_SEARCHINFO *info)
{
   // iterative deepening , search init
   
   // for depth =1 to maxDepth
        // search AlphaBeta
        // next depth

  int bestmove = NOMOVE;
  int bestscore = -INFINITE;
  int currentdepth =0;
  int pvmoves = 0;
  int pvnum =0;
  clearforsearch(pos,info);
   
  // iterative deepening
  for(currentdepth = 1; currentdepth <= info->depth; currentdepth++)
  {
    //alpha beta
    bestscore = alphabeta(-INFINITE,INFINITE,currentdepth,pos,info,TRUE);
    
    // out of time?
    
    pvmoves = getpvline(currentdepth,pos);
    bestmove = pos->pvarray[0]; 

    printf("Depth: %d score:%d move: %s nodes : %ld",currentdepth,bestscore,prmove(bestmove),info->nodes);
    pvmoves = getpvline(currentdepth,pos);
    printf("pv");
    for(pvnum=0;pvnum < pvmoves;pvnum++)
      printf("%s ",prmove(pos->pvarray[pvnum]));
    printf("\n");
    printf("Ordering:%2f\n",(info->fhf/info->fh));
  }
}
