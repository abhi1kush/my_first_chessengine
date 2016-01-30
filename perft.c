//perft.c

#include "stdio.h"
#include <string.h>
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
    //printf(" mv%4s",prmove(list->moves[movenum].move));
    perft(depth - 1, pos);
    takemove(pos);
  }
  return ;
}



void perfttest(int depth, S_BOARD *pos)
{
  int d=6;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  char buf[1000];
  FILE *fptr,*fp;

  fptr = fopen("out","w");
  if(fptr==NULL)
  {
    printf("w ERROR");
    exit(1);
  }

  fp = fopen("fens","r");
  if (fp == NULL)
  {
    printf("r ERROR");
    exit(1);
  }
  
  while ((read = getline(&line, &len, fp)) != -1) 
  {
      line[strlen(line)-1]='\0';
      fprintf(fptr,"%s;",line);
      //printf("%s;",line);
      parse_fen(line,pos);
      //for(d=1;d<=depth;d++)
      {
        ASSERT(checkboard(pos));
        //printboard(pos);
        fprintf(fptr,"D%d ",d);
        //printf(" D%d ",d);

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
          perft(d-1, pos);
          takemove(pos);
          long oldnodes = leafnodes - cumnodes;
          //printf("move %d : %s : %ld\n",movenum+1,prmove(move),oldnodes);

        }
        fprintf(fptr,"%ld ; ",leafnodes);  
        //printf("%ld; ",leafnodes);  
      }
      fprintf(fptr,"\n");
      //printf("\n");
  }
  fclose(fp);
  fclose(fptr);
  if (line)
     free(line);
  return;
}
