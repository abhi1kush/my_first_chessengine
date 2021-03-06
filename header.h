//header.h
#ifndef DEFS_H
#define DEFS_H
#include "stdlib.h"
#include <stdio.h>

//#define DEBUG

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
    printf("In function %s\n ",__func__); \
    exit(1);}
#endif

typedef unsigned long long u64;

#define NAME "abhi_engine 1.0"
#define SQ_NUM 120
#define MAXMOVES 2048
#define MAXPOSITIONMOVES 256
#define MAXDEPTH 64 
#define MAX_HASH 2048
#define SETDEPTH 6

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

#define INFINITE 30000
#define ISMATE (INFINITE - MAXDEPTH)

enum { EMPTY, wp, wn, wb, wr, wq, wk, bp, bn, bb, br, bq, bk };
enum {FILE_A, FILE_B, FILE_C, FILE_D ,FILE_E ,FILE_F ,FILE_G ,FILE_H, FILE_NONE };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_NONE };
enum {UCIMODE, XBOARDMODE, CONSOLEMODE};

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
	A8=91, B8, C8, D8, E8, F8, G8, H8, NO_SQ,OFFBOARD};

enum {FALSE, TRUE};
enum { HFNONE, HFALPHA, HFBETA, HFEXACT };

typedef struct {
  int move;
  int score;

}S_MOVE;

typedef struct {
  S_MOVE moves[MAXPOSITIONMOVES];
  int count;
}S_MOVELIST;

typedef struct {
  u64 poskey;
  int move;
  int score;
  int depth;
  int flags;
}S_HASHENTRY;

typedef struct {
  S_HASHENTRY *ptable;
  int numentries;
  int newwrite;
  int overwrite;
  int hit;
  int cut;
}S_HASHTABLE;

typedef struct {
  int move;
  int castle;
  int enpass;
  int fiftymove;
  u64 poskey;
}S_UNDO;

typedef struct {
   int  pieces[SQ_NUM];
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
   int bigpce[2]; //anything but pawn
   int majpce[2];
   int minpce[2];
   int material[2];
   S_UNDO history[MAXMOVES];

   //piece list
   int plist[13][10];

   // plist[wn][0]=E1;
   // plist[wn][1]=D4; .. .. ..

   S_HASHTABLE hashtable[1];
   int pvarray[MAXDEPTH]; 

   int searchhistory[13][SQ_NUM]; 
   int searchkillers[2][MAXDEPTH];

}S_BOARD;

typedef struct {

  unsigned long starttime;
  unsigned long stoptime;
  int depth;
  int depthset;
  int timeset;
  int movestogo;
  int infinite;

  long nodes;

  int quit;
  int stopped;

  float fh;
  float fhf;
  int nullcut;

  int GAME_MODE;
  int POST_THINKING;

}S_SEARCHINFO;

typedef struct {
  int usebook; 
}S_OPTIONS;


/*GAME MOVE*/
/*                          7    f
0000 0000 0000 0000 0000 0111 1111 -> from  0x7f
0000 0000 0000 0011 1111 1000 0000 -> to    >> 7, 0x7f
0000 0000 0011 1100 0000 0000 0000 -> captured >> 14 , 0xf
0000 0000 0100 0000 0000 0000 0000 -> ep 0x40000
0000 0000 1000 0000 0000 0000 0000 -> pawn start 0x80000
0000 1111 0000 0000 0000 0000 0000 -> promoted >> 20, 0xf
0001 0000 0000 0000 0000 0000 0000 -> castle 0x100000
*/

#define FROMSQ(m) ((m) & 0x7F)
#define TOSQ(m)  (((m)>>7) & 0x7F)
#define CAPTURED(m) (((m)>>14) & 0xF)
#define PROMOTED(m) (((m)>>20) & 0xF) 
#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000

#define MFLAGCAP 0x7C000
#define MFLAGPROM 0xF00000

#define NOMOVE 0

/* macros */

#define FR2SQ(f,r) ((21 +(f))+((r)*10))
#define SQ64(n) sq120tosq64[n]
#define SQ120(n) sq64tosq120[n]
#define POP(n) popbit(n)
#define CNT(n) countbits(n)
#define clrbit(bitb,sq) ( (bitb) &= clearmask[sq] )
#define setbit(bitb,sq) ( (bitb) |= setmask[sq] )

#define isbq(p) (piecebishopqueen[(p)])
#define isrq(p) (piecerookqueen[(p)])
#define iskn(p) (pieceknight[(p)])
#define iski(p) (pieceking[(p)])

/* global */

extern int sq120tosq64[SQ_NUM];
extern int sq64tosq120[64];
extern u64 setmask[64];
extern u64 clearmask[64];
extern u64 piecekeys[13][120];
extern u64 sidekey;
extern u64 castlekeys[16];

extern char pcechar[];
extern char sidechar[];
extern char filechar[];
extern char rankchar[];

extern int piecebig[13];
extern int piecemaj[13];
extern int piecemin[13];
extern int pieceval[13];
extern int piececol[13];
extern int piecepawn[13];

extern const int mirror64[64];

extern int filesbrd[SQ_NUM];
extern int ranksbrd[SQ_NUM];

extern int pieceknight[13];
extern int pieceking[13];
extern int piecerookqueen[13];
extern int piecebishopqueen[13];

extern u64 filebbmask[8];
extern u64 rankbbmask[8];

extern u64 blackpassedmask[64];
extern u64 whitepassedmask[64];
extern u64 isolatedmask[64];
extern S_OPTIONS engineoptions[1];

/* function */

//init.c
extern void Allinit();
//extern void inithashkeys();

//bitboard.c
extern void printbitboard(u64 bitb);
extern int popbit(u64 *bitb);
extern int countbits(u64 bitb);

//hashkeys.c
extern u64 generateposkey(const S_BOARD *pos);

//board.c
extern void resetboard(S_BOARD *pos);
extern int parse_fen(char *fen,S_BOARD *pos);
extern void printboard(const S_BOARD * pos);
extern void updatelistmaterial(S_BOARD *pos);
extern int checkboard(const S_BOARD *pos);

//attack.c
extern int sqattacked(const int sq, const side, const S_BOARD *pos);

//io.c
extern char * prmove(const int move);
extern char * prsq(const int sq);
extern void printmovelist(const S_MOVELIST *list);
extern int parsemove(char * ch,S_BOARD *p);
//movegen.c
extern void generateallmoves(const S_BOARD *pos,S_MOVELIST *list);
extern int moveexists(S_BOARD *pos,const int move);
extern int InitMvvLva();
extern void generateallcaps(const S_BOARD* ,S_MOVELIST *);
//makemove.c
extern int makemove(S_BOARD *pos,int move);
extern void takemove(S_BOARD *pos);
extern void makenullmove(S_BOARD *);
extern void takenullmove(S_BOARD *);
//validate.c
extern int sqonboard(const int sq);
extern int sidevalid(const int side);
extern int filerankvalid(const int fr);
extern int piecevalidempty(const int pce);
extern int piecevalid(const int pce);
extern int movelistok(const S_MOVELIST *,const S_BOARD *);
extern int sqis120(const int sq);
extern void DebugAnalysisTest(S_BOARD *pos,S_SEARCHINFO *info);
// perft.c
extern void perfttest(int depth, S_BOARD *pos);

//search.c
extern void searchposition(S_BOARD *,S_SEARCHINFO *);

//misc.c
extern int gettime();
extern void readinput(S_SEARCHINFO *info);

//pvtable.c
extern void inithashtable(S_HASHTABLE *table,const int);
extern void storehashentry(S_BOARD *pos, const int move,int score,const int flags,const depth);
extern int probehashentry(S_BOARD *pos,int *move,int *score, int alpha,int beta,int depth);
extern int getpvline(const int depth, S_BOARD *pos);
extern void clearhashtable(S_HASHTABLE *table);
extern int probepvmove(const S_BOARD *pos);

//evaluate.c
extern int evalposition(const S_BOARD *pos);

//uci.c
extern void uci_loop(S_BOARD *pos, S_SEARCHINFO *info); 

//xboard.c
extern void console_loop(S_BOARD *pos, S_SEARCHINFO *info);
extern void xboard_loop(S_BOARD *pos, S_SEARCHINFO *info);

//polybook.c
extern u64 polykeyfromboard(const S_BOARD *);
extern void initpolybook();
extern void cleanpolybook();
extern int getbookmove(S_BOARD * board);

#endif
