// search.c

#include <stdio.h>
#include "header.h"

int isrepetition(const S_BOARD *pos)
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
