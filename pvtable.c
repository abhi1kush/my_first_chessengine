//pvtable.c

#include <stdio.h>
#include "header.h"

int getpvline(const int depth, S_BOARD *pos)
{
  ASSERT(depth<MAXDEPTH);

  int move = probepvmove(pos);
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
    move = probepvmove(pos);
  }
  
   while(pos->ply > 0)
    takemove(pos);
  
  return count;

}




const int hashsize = 0x100000 * 16;

void clearhashtable(S_HASHTABLE *table)
{
  S_HASHENTRY * tableentry;
  for(tableentry = table->ptable; tableentry < table->ptable + table->numentries; tableentry++)
  {
    tableentry->poskey = 0ULL;
    tableentry->move = NOMOVE;
    tableentry->depth = 0;
    tableentry->score = 0;
    tableentry->flags = 0;
  }
  table->newwrite=0;  
}

void inithashtable(S_HASHTABLE *table,const int MB)
{
  int hashsize = 0x100000 * MB;
  table->numentries = hashsize/sizeof(S_HASHENTRY);
  table->numentries -= 2;
  
  if(table->ptable!=NULL)
    free(table->ptable);

  table->ptable = (S_HASHENTRY *)malloc(table->numentries*sizeof(S_HASHENTRY));
  
  if(table->ptable==NULL)
  {
    printf("hashTable Allocation failed, trying  %dMB ...\n",MB/2);
    inithashtable(table,MB/2);
  }
  else
  {
    clearhashtable(table);
    printf("HashTable init complete with %d entries \n",table->numentries);
  }
}

void storehashentry(S_BOARD *pos, const int move,int score,const int flags,const depth)
{
  int i = pos->poskey % pos->hashtable->numentries;

  ASSERT(i >= 0 && i <= pos->hashtable->numentries - 1);
  ASSERT(depth >= 1 && depth < MAXDEPTH);
  ASSERT(flags >= HFALPHA && flags<=HFEXACT);
  ASSERT(score >= -INFINITE && score <= INFINITE);
  ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);

  if(pos->hashtable->ptable[i].poskey == 0)
    pos->hashtable->newwrite++;
  else
    pos->hashtable->overwrite++;

  if(score > ISMATE) 
    score += pos->ply;
  else if(score < -ISMATE)
    score -=pos->ply;

  pos->hashtable->ptable[i].move = move;
  pos->hashtable->ptable[i].poskey = pos->poskey;
  pos->hashtable->ptable[i].flags = flags;
  pos->hashtable->ptable[i].score = score;
  pos->hashtable->ptable[i].depth = depth;
  
}

int probehashentry(S_BOARD *pos,int *move,int *score, int alpha,int beta,int depth)
{
  int i =pos->poskey % pos->hashtable->numentries;

  ASSERT(i >= 0 && i <= pos->hashtable->numentries - 1);
  ASSERT(depth >= 1 && depth < MAXDEPTH );
  ASSERT(alpha < beta);
  ASSERT(alpha >= -INFINITE && alpha <= INFINITE);
  ASSERT(beta >= -INFINITE && beta <= INFINITE);
  ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);

  if(pos->hashtable->ptable[i].poskey == pos->poskey)
  {
    *move = pos->hashtable->ptable[i].move;
    if(pos->hashtable->ptable[i].depth >= depth)
    {
      pos->hashtable->hit++;

      ASSERT(pos->hashtable->ptable[i].depth >= 1 && pos->hashtable->ptable[i].depth < MAXDEPTH);
      ASSERT(pos->hashtable->ptable[i].flags>=HFALPHA && pos->hashtable->ptable[i].flags <= HFEXACT);
      
      *score = pos->hashtable->ptable[i].score;
      if(*score > ISMATE) 
        *score -= pos->ply;
      else if(*score < -ISMATE) 
        *score += pos->ply;
      
      switch(pos->hashtable->ptable[i].flags)
      {
        ASSERT(*score >= -INFINITE && *score <= INFINITE);

        case HFALPHA: if(*score <=alpha)
                      {
                        *score = alpha;
                        return TRUE;
                      }
                      break;
        case HFBETA: if(*score >= beta)
                     {
                       *score = beta;
                       return TRUE;
                     }
                     break;
        case HFEXACT:
                     return TRUE;
                     break;
        default : ASSERT(FALSE);
      }
    }
    return pos->hashtable->ptable[i].move;
  }
  
  return NOMOVE;
}

int probepvmove(const S_BOARD *pos) 
{
    int i = pos->poskey % pos->hashtable->numentries;
    ASSERT(i >= 0 && i <= pos->hashtable->numentries - 1);
    if( pos->hashtable->ptable[i].poskey == pos->poskey ) 
    {
      return pos->hashtable->ptable[i].move;
    }
    return NOMOVE;
}


