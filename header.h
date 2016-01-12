#ifndef DEFS_H
#define DEFS_H
#include "stdlib.h"

#define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n) \
  if(!(n)) { \
    printf("%s - Failed ",#n); \
    printf("On %s ",__DATE__); \
    printf("At %s ",__TIME__); \
    printf("IN File %s ",__FILE__); \
    printf("At Line %d\n ",__LINE__); \
    exit(1);}
#endif

typedef unsigned long long u64;

#define NAME "engine 1.0"
#define SQ_NUM 120
#define MAXMOVES 2048

enum { EMPTY, wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk };
enum {FILE_A, FILE_B, FILE_C, FILE_D ,FILE_E ,FILE_F ,FILE_G ,FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };

enum { WHITE, BLACK, BOTH };

// 0000
enum {WO_O=1, WO_O_O=2,BO_O=4,BO_O_O=8};

enum {
	A1=21, B1, C1, D1, E1, F1, G1, H1,
	A2=31, B2, C2, D2, E2, F2, G2 ,H2,
	A3=41, B3, C3, D3, E3 ,F3, G3, H3,
	A4=51, B4, C4, D4, E4, F4, G4, H4,
	A5=61, B5, C5, D5, E5, F5, G5, H5,
	A6=71, B6, C6, D6, E6, F6, G6, H6,
	A7=81, B7, C7, D7, E7, F7, G7, H7,
	A8=91, B8, C8, D8, E8, F8, G8, H8, NO_SQ};
enum {FALSE, TRUE};

typedef struct {
  int move;
  int castle;
  int enpass;
  int fiftymove;
  u64 poskey;
}S_UNDO;

typedef struct {
  int  piecesp[SQ_NUM];
  u64 pawns[3]; //01000000 00000000 00000000 ......
   int kingsq[2];

   int side;
   int enpass;
   int fiftymove;
   int castle;

   int ply;
   int historyply;
   u64 poskey;
   int pceNum[13];
   int bigpce[3]; //anything but pawn
   int majpce[3];
   int minpce[3];
   S_UNDO history[MAXMOVES];

   //piece list
   int plist[13][10];

   // plist[wn][0]=E1;
   // plist[wn][1]=D4; .. .. ..

}S_BOARD;

/* macros */

#define FR2SQ(f,r) ((21 +(f))+((r)*10))
/* global */

extern int sq120tosq64[SQ_NUM];
extern int sq64tosq120[64];

/* function */
void Allinit();
//init.c





#endif
