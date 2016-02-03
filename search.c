// search.c

#include <stdio.h>
#include "header.h"

#define INFINITE 30000
#define MATE 29000

static void picknextmove(int movenum, S_MOVELIST *list)
{
  S_MOVE temp;
  int i=0;
  int bestscore = 0;
  int bestnum = movenum;

  for(i = movenum;i < list->count; i++)
    if(list->moves[i].score > bestscore)
    {
      bestscore = list->moves[i].score;
      bestnum = i;
    }
  temp = list->moves[movenum];
  list->moves[movenum] = list->moves[bestnum];
  list->moves[bestnum] = temp;
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

static void checkup(S_SEARCHINFO *info)
{
  //check if time up, or interrupt from GUI
  if(info->timeset == TRUE && gettime() > info->stoptime)
    info->stopped = TRUE;
}

static int quiescence(int alpha, int beta, S_BOARD *pos,S_SEARCHINFO *info)
{
  ASSERT(checkboard(pos));

  if((info->nodes & 2047) == 0 )
    checkup(info);

  info->nodes++;
  
  if(isrepetition(pos) || pos->fiftymove >= 100)
  {
    return 0;
  }

  if(pos->ply > MAXDEPTH-1)
    return evalposition(pos);

  int score = evalposition(pos);

  if(score >= beta)
    return beta;

  if(score > alpha)
    alpha = score;
  
  S_MOVELIST list[1];
  generateallcaps(pos,list);

  int movenum = 0;
  int legal =0;
  int oldalpha = alpha;
  int bestmove = NOMOVE;
  score = -INFINITE;
  int pvmove = probepvtable(pos);
  
  for(movenum=0; movenum < list->count; ++movenum)
  {
    picknextmove(movenum,list);

    if(!makemove(pos,list->moves[movenum].move))
      continue;
    legal++;
    score = -quiescence(-beta,-alpha,pos,info);
    takemove(pos);

    if(info->stopped == TRUE)
      return 0;
    
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
  


  if(alpha != oldalpha)
    storepvmove(pos,bestmove);
  
  return alpha;
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
    return quiescence(alpha,beta,pos,info);
    //info->nodes++;
    //return evalposition(pos);
  }
  
  if((info->nodes & 2047) == 0 )
    checkup(info);

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
  int pvmove = probepvtable(pos);

  if(pvmove != NOMOVE)
  {
    for(movenum=0; movenum < list->count; movenum++ )
    {
      if(list->moves[movenum].move==pvmove)
      {
        list->moves[movenum].score = 2000000;
        break;
      }
    }
  }

  for(movenum=0; movenum < list->count; ++movenum)
  {
    picknextmove(movenum,list);

    if(!makemove(pos,list->moves[movenum].move))
      continue;
    legal++;
    score = -alphabeta(-beta,-alpha,depth - 1,pos,info,TRUE);
    takemove(pos);
    
    if(info->stopped == TRUE)
      return 0;
    
    if(score > alpha)
    {
      if(score >= beta)
      {
        if(legal ==1)
          info->fhf++;
        
        info->fh++;

        if(!(list->moves[movenum].move & MFLAGCAP))
        {
          pos->searchkillers[1][pos->ply] = pos->searchkillers[0][pos->ply];
          pos->searchkillers[0][pos->ply] = list->moves[movenum].move;
        }
        return beta;
      }
      alpha = score;
      bestmove = list->moves[movenum].move;
      if(!(list->moves[movenum].move & MFLAGCAP))
      {
          pos->searchhistory[pos->pieces[FROMSQ(bestmove)]][TOSQ(bestmove)] += depth;
      }
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
    if(info->stopped == TRUE)
      break;

    pvmoves = getpvline(currentdepth,pos);
    bestmove = pos->pvarray[0]; 

    printf("info score cp %d depth %d nodes %ld time %d ",bestscore,currentdepth,info->nodes,gettime()-info->starttime);
    pvmoves = getpvline(currentdepth,pos);
    printf("pv");
    for(pvnum=0;pvnum < pvmoves;pvnum++)
      printf(" %s \n",prmove(pos->pvarray[pvnum]));
    printf("\n");
    printf("Ordering:%2f\n",(info->fhf/info->fh));
  }
  //info score cp 13 depth 1 nodes 13 time 15 pv f1b5
  printf("bestmove %s \n",prmove(bestmove));
}
