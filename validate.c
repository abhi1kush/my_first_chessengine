//validate.c

#include "header.h"
#include <stdio.h>
#include <string.h>

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

int movelistok(const S_MOVELIST *list, const S_BOARD *pos)
{
  if(list->count < 0 || list->count >= MAXMOVES )
    return FALSE;
  int movenum;
  int from = 0;
  int to = 0;
  for(movenum = 0;movenum < list->count; movenum++)
  {
    to= TOSQ(list->moves[movenum].move);
    from = FROMSQ(list->moves[movenum].move);
    if(!sqonboard(to) || !sqonboard(from))
    {
      return FALSE;
    }
    if(!piecevalid(pos->pieces[from]))
    {
      printboard(pos);
      return FALSE;
    }
  }
  return TRUE;
}

int sqis120(const int sq)
{
  return (sq>= 0 && sq<120);
}

void DebugAnalysisTest(S_BOARD *pos, S_SEARCHINFO *info) 
{

  FILE *file;

    file = fopen("lct2.epd","r");

    char lineIn [1024];



  info->depth = MAXDEPTH;

  info->timeset = TRUE;

  int time = 1140000;





    if(file == NULL) {

        printf("File Not Found\n");

        return;

    }  else {

        while(fgets (lineIn , 1024 , file) != NULL) {

      info->starttime = gettime();

      info->stoptime = info->starttime + time;

      clearhashtable(pos->hashtable);

            parse_fen(lineIn, pos);

            printf("\n%s\n",lineIn);

      printf("time:%d start:%ld stop:%ld depth:%d timeset:%d\n",

        time,info->starttime,info->stoptime,info->depth,info->timeset);

      searchposition(pos, info);

            memset(&lineIn[0], 0, sizeof(lineIn));

        }

    }

}


/*
void MirrorEvalTest(S_BOARD *pos) {
    FILE *file;
    file = fopen("mirror.epd","r");
    char lineIn [1024];
    int ev1 = 0; int ev2 = 0;
    int positions = 0;
    if(file == NULL) {
        printf("File Not Found\n");
        return;
    }  else {
        while(fgets (lineIn , 1024 , file) != NULL) {
            ParseFen(lineIn, pos);
            positions++;
            ev1 = EvalPosition(pos);
            MirrorBoard(pos);
            ev2 = EvalPosition(pos);

            if(ev1 != ev2) {
                printf("\n\n\n");
                ParseFen(lineIn, pos);
                PrintBoard(pos);
                MirrorBoard(pos);
                PrintBoard(pos);
                printf("\n\nMirror Fail:\n%s\n",lineIn);
                getchar();
                return;
            }

            if( (positions % 1000) == 0)   {
                printf("position %d\n",positions);
            }

            memset(&lineIn[0], 0, sizeof(lineIn));
        }
    }
}
*/
