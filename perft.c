//perft.c

#include "stdio.h"
#include "header.h"

long leafnodes;

void perft(int depth, S_BOARD *pos)
{
  char buf[1000];
  FILE *fptr;
  fptr = fopen("out","w");
  if(fptr==NULL)
  {
    printf("ERROR");
    exit(1);
  }

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
    //printf(" mv%4s",prmove(list->moves[movenum].move));
    perft(depth - 1, pos);
    takemove(pos);
  }
  return ;
}

FILE * fp;
char * line = NULL;
size_t len = 0;
ssize_t read;


void perfttest(int depth, S_BOARD *pos)
{
  int d;
  fp = fopen("fens", "r");
  if (fp == NULL)
      exit(EXIT_FAILURE);
  while ((read = getline(&line, &len, fp)) != -1) {
      printf("Retrieved line of length %zu :\n", read);
      parse_fen(line,board);
      perfttest(5,board);
      }

  fclose(fp);
  if (line)
     free(line);
  
  ASSERT(checkboard(pos));
  //printboard(pos);
  fprintf(fptr,"Depth : %d :",depth);
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
    //printf("move %d : %s : %ld\n",movenum+1,prmove(move),oldnodes);

  }
  fprintf(" %ld nodes visited ; ",leafnodes);
  }
  return;
}
