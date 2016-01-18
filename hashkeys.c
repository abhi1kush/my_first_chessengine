//hashkey.c
#include "header.h"
#include <stdio.h>

u64 generateposkey(const S_BOARD *pos)
{
  int sq=0;
  u64 finalkey =0;
  int piece = EMPTY;
  
  printf("0 poskey  %llx \n",finalkey);
  

  //pieces 
  for(sq=0;sq<SQ_NUM ; ++sq)
  {
    piece = pos->pieces[sq];
    if(piece != OFFBOARD && piece!= EMPTY)
    {
      ASSERT(piece >= wp && piece <= bk);
      finalkey ^=  piecekeys[piece][sq];
      printf("1 poskey (^piece %d sq %d) %llx \n",piece,sq,finalkey);
    }
  }

  if(pos->side == WHITE)
  {
    finalkey ^= sidekey;
    printf("2 poskey (^sidekey) %llx \n",finalkey);
  }
  if(pos->enpass != NO_SQ)
  {
    ASSERT( pos->enpass >= 0 && pos->enpass <SQ_NUM);
    finalkey ^= piecekeys[EMPTY][pos->enpass];
    printf("3 poskey (^enpass) %llx \n",finalkey);
  }

  ASSERT( pos->castle >= 0 && pos->castle <= 15);
  finalkey ^= castlekeys[pos->castle];
  printf("4 poskey (^castle) %llx \n",finalkey);

  return finalkey;

}
