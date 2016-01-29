//perft.c

#include "stdio.h"
#include "header.h"

long leafnodes;

void perft(int depth, S_BOARD *pos)
{
  ASSERT(checkboard(pos));
  //printf("depth %d ",depth);
  if(depth==0)
  {
    leafnodes++;
    //printf(" *check* ");
    return;
  }

  S_MOVELIST list[1];
  generateallmoves(pos,list);

  int movenum =0;
  for( movenum =0; movenum < list->count ; movenum++)
  {
    if( !makemove(pos,list->moves[movenum].move) )
      continue;
    perft(depth - 1, pos);
    takemove(pos);
  }
  return ;
}

void perfttest(int depth, S_BOARD *pos)
{
  ASSERT(checkboard(pos));
  printboard(pos);
  printf("\nStarting Test To Depth : %d\n",depth);
  leafnodes =0;

  S_MOVELIST list[1];
  generateallmoves(pos,list);

  int move ;
  int movenum =0;
  for(movenum =0; movenum < list->count ; ++movenum)
  {
    move = list->moves[movenum].move;
    if(!makemove(pos,move))
      continue;
    long cumnodes =leafnodes;
    perft(depth -1, pos);
    takemove(pos);
    long oldnodes = leafnodes - cumnodes;
    printf("move %d : %s : %ld\n",movenum+1,prmove(move),oldnodes);

  }
  printf("\n Test Complete : %ld nodes visited \n",leafnodes);
  return;
}
