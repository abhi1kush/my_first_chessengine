//hashkey.c
#include "header.h"

u64 generateposkey(const S_BOARD *pos)
{
  int sq=0;
  u64 finalkey =0;
  int piece = EMPTY;
  //pieces
  for(sq=0;sq<SQ_NUM ; ++sq)
  {
    piece = pos->pieces[sq];
    if(piece != NO_SQ && piece!= EMPTY)
    {
      ASSERT(piece>=wp&&piece<=bk);
      finalkey ^=  piecekeys[piece][sq];
    }
  }

  if(pos->side == WHITE)
    finalkey ^=sidekey;
  if(pos->enpass != NO_SQ)
  {
    ASSERT(pos->enpass>=0 && pos->enpass<SQ_NUM);
    finalkey ^= piecekeys[EMPTY][pos->enpass];
  }

  ASSERT(pos->castle>=0&&pos->castle<=15);
  finalkey ^= castlekeys[pos->castle];

  return finalkey;

}
