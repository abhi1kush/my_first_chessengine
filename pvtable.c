//pvtable.c

#include <stdio.h>
#include "header.h"


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
  free(table->ptable);
  table->ptable = (S_PVENTRY *)malloc(table->numentries*sizeof(S_PVENTRY));
  clearpvtable(table);
  printf("pvTable init complete with %d entries \n",table->numentries);
}

