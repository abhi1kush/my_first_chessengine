//attack.c

#include "header.h"
#include <stdio.h>

const int kndir[8] = { -8,-19,-21,-12,8,19,21,12};
const int rkdir[4] = {-1,-10,1,10};
const int bidir[4] = {-9,-11,11,9};
const int kidir[8] = {-1,-10,1,10,-9,-11,11,9};

int sqattacked(const int sq, const int side, const S_BOARD *pos)
{
  int pce,i,t_sq,dir;

  ASSERT(sqonboard(sq));
  ASSERT(sidevalid(side));
  //printf("assert checkboard in sqattacked");
  ASSERT(checkboard(pos));

  //pawns
  if(side==WHITE)
  {
    if(pos->pieces[sq-11]==wp || pos->pieces[sq-9] == wp)
      return TRUE;
  }
  else
  {
    if(pos->pieces[sq-11]==bp || pos->pieces[sq-9] == bp)
      return TRUE;

  }
  
  //knights
  for(i=0;i < 8;i++)
  {
    pce=pos->pieces[sq+kndir[i]];
    if(iskn(pce) && piececol[pce] == side)
      return TRUE;
  }
  
  //rooks,queens
  for(i=0;i<4;i++)
  {
    dir=rkdir[i];
    t_sq = sq + dir;
    pce = pos->pieces[t_sq];
    while(pce!=OFFBOARD)
    {
      if(pce != EMPTY)
      {
        if(isrq(pce) && piececol[pce] == side)
          return TRUE;
        break;
      }
    t_sq +=dir;
    pce = pos->pieces[t_sq];
    }
  }

 // bishop , queens
 for(i=0;i<4;i++)
 {
    dir = bidir[i];
    t_sq = sq +dir;
    pce = pos->pieces[t_sq];
    while(pce != OFFBOARD)
    {
      if(pce != EMPTY)
      {
        if(isbq(pce) && piececol[pce] == side)
          return TRUE;
        break;
      }
      t_sq += dir;
      pce = pos->pieces[t_sq];
    }
  }
  
  //kings
  for(i=0;i<8;++i)
  {
    pce=pos->pieces[sq + kidir[i]];
    if(iski(pce) && piececol[pce] == side)
      return TRUE;
  }

  return FALSE;
}

