//pvtable.c

#include <stdio.h>
#include "header.h"

int getpvline(const int depth, S_BOARD *pos)
{
  ASSERT(depth<MAXDEPTH);

  int move = probepvtable(pos);
  int count = 0;
  while(move != NOMOVE && count < depth)
  {
    ASSERT(count < MAXDEPTH);
    
    if(moveexists(pos,move))
    {
      makemove(pos,move);
      pos->pvarray[count++] = move;
    }
    else
      break;
    move = probepvtable(pos);
  }
  
   while(pos->ply > 0)
    takemove(pos);
  
  return count;

}




const int pvsize = 0x100000 * 2;

void clearpvtable(S_PVTABLE *table)
{
  S_PVENTRY *pventry;
  for(pventry = table->ptable; pventry < table->ptable + table->numentries; pventry++)
  {
    pventry->poskey = 0ULL;
    pventry->move = NOMOVE;
  }
   
}

void initpvtable(S_PVTABLE *table)
{
  table->numentries = pvsize/sizeof(S_PVENTRY);
  table->numentries -= 2;
  //if(table->ptable != NULL) {free(table->ptable);}
  table->ptable = (S_PVENTRY *)malloc(table->numentries*sizeof(S_PVENTRY));
  clearpvtable(table);
  printf("pvTable init complete with %d entries \n",table->numentries);
}

void storepvmove(const S_BOARD *pos, const int move)
{
  int i = pos->poskey % pos->pvtable->numentries;
  ASSERT(i >= 0 && i <= pos->pvtable->numentries - 1);

  pos->pvtable->ptable[i].move = move;
  pos->pvtable->ptable[i].poskey = pos->poskey;  

}

int probepvtable(const S_BOARD *pos)
{
  int i =pos->poskey % pos->pvtable->numentries;
  ASSERT(i >= 0 && i <= pos->pvtable->numentries - 1);

  if(pos->pvtable->ptable[i].poskey == pos->poskey)
    return pos->pvtable->ptable[i].move;
  
  return NOMOVE;
}

