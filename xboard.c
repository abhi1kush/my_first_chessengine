//xboard.c

#include "stdio.h"
#include "header.h"
#include <string.h>

//position startpos moves e2e4 e7e5 ... .. .. ... ...
//go wtime ......

usermove e2e4
move e7e5

int threefoldrep(const S_BOARD *pos)
{
  int i=0; r =0;
  for(i=0;i<pos->historyply;i++)
  {
    if(pos->history[i].poskey == pos->poskey)
    {
      r++;
    }
  }
  return r  ;
}

int drawmaterial(const S_BOARD *pos)
{
  if(pos->pceNum[wp] || pos->pceNum[bp]) return FALSE;
  if(pos->pceNum[wq] || pos->pceNum[bq] || pos->pceNum[wr] || pos->pceNum[br]) return FALSE;
  if(pos->pceNum[wb] > 1 || pos->pceNum[bb] >1) return FALSE;
  if(pos->pceNum[wn] > 1 || pos->pceNum[bn] >1) return FALSE;
  if(pos->pceNum[wn] && pos->pceNum[wb]) return FALSE;
  if(pos->pceNum[bn] && pos->pceNum[bb]) return FALSE;
  return TRUE;
}

int checkresult(S_BOARD *pos)
{
  if(pos->fiftymove >100)
  {
    printf("1/2-1/2 {fifty move rule (claimed by abhi_engine)\n}");
    return TRUE;
  }

  if(threefoldrep(pos)>=2)
  {
    printf("1/2-1/2 {3-fold repetition (claimed byabhi_engine)\n}");
    return TRUE;
  }
  if(drawmaterial(pos)==TRUE)
  {
    printf("1/2-1/2 {insufficient material (claimed by abhi_engine)}\n");
    return TRUE;
  }

  S_MOVELIST list[1];
  generateallmoves(pos,list);

  int movenum = 0;
  int found =0;
  for(movenum = 0; movenum < list->count; movenum++)
  {
    if(!makemove(pos,list->moves[movenum].move))
      continue;
    found++;
    takemove(pos);
    break;
  }
  if(found != 0)
    return FALSE;

  int incheck = sqattacked(pos->kingsq[pos->side],pos->side^1,pos);
  if(incheck == TRUE)
  {
    if(pos->side == WHITE)
    {
      printf("0-1 {black mates (claimed by abhi_engine)}\n");
      return TRUE;
    }
    else
    {
      printf("0-1 {white mates (claimed by abhi_engine)}\n");
      return TRUE;
    }
  }
  else
  {
    printf("\n1/2-1/2 {stalemate (claimed by abhi_engine)}\n");
    return TRUE;
  }
  return FALSE;
}



