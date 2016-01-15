//validate.c

#include "header.h"

int sqonboard(const int sq)
{
  return filesbrd[sq]==OFFBOARD ? 0:1;
}

int sidevalid(const int side)
{
  return (side==WHITE || side == BLACK) ? 1 : 0;
}

int filrankvalid(const int fr)
{
  return (fr >= 0 && fr <= 7) ? 1 : 0;
}

int piecevalidempty(const int pce)
{
  return (pce >= EMPTY && pce <= bk) ? 1:0;
}

int piecevalid(const int pce)
{
  return (pce >= wp && pce <= bk) ? 1:0;
}
