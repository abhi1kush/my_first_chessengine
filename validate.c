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
