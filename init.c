#include "header.h"
#include "stdlib.h"
#include <stdio.h>
// 0000 0000000000000 0000000000000 0000000000000000 1111111111111

#define RAND_64 ( (u64)rand() |\
                  (u64)rand() <<15 |\
                  (u64)rand() <<30 |\
                  (u64)rand() <<45 |\
                  ((u64)rand() &0xf) <<60 )
int sq120tosq64[SQ_NUM];
int sq64tosq120[64];

u64 setmask[64];
u64 clearmask[64];

u64 piecekeys[13][120];
u64 sidekey;
// 0 0 0 0
u64 castlekeys[16];

int filesbrd[SQ_NUM];
int ranksbrd[SQ_NUM];
u64 filebbmask[8];
u64 rankbbmask[8];

u64 blackpassedmask[64];
u64 whitepassedmask[64];
u64 isolatedmask[64];

/*
 0 0 0 1 1 1 0 0
 0 0 0 1 1 1 0 0
 0 0 0 1 1 1 0 0
 0 0 0 1 1 1 0 0
 0 0 0 1 1 1 0 0
 0 0 0 0 x 0 0 0
 0 0 0 0 0 0 0 0
 0 0 0 0 0 0 0 0
 */

void initevalmasks()
{
  int sq, tsq, r, f;
  for(sq=0;sq<8;sq++)
  {
    filebbmask[sq] = 0ULL;
    rankbbmask[sq] = 0ULL;
  }

  for(r = RANK_8;r >= RANK_1; r--)
  {
    for(f = FILE_A;f <= FILE_H; f++)
    {
      sq = r*8+f;
      filebbmask[f] |= (1ULL << sq);
      rankbbmask[r] |= (1ULL << sq);
    }
  }
  
  for(sq = 0; sq<64; sq++)
  {
    isolatedmask[sq] = 0ULL;
    whitepassedmask[sq] = 0ULL;
    blackpassedmask[sq] = 0ULL;
  }
  
  for(sq = 0; sq < 64; sq++)
  {
    tsq = sq + 8;
    while(tsq < 64)
    {
      whitepassedmask[sq] |= (1ULL << tsq);
      tsq+= 8;
    }

    tsq = sq -8;
    while(tsq >= 0)
    {
      blackpassedmask[sq] != (1ULL << tsq);
      tsq -= 8;
    }

    if(filesbrd[SQ120(sq)] > FILE_A)
    {
      isolatedmask[sq] |= filebbmask[filesbrd[SQ120(sq)] - 1];
      tsq = sq + 7;
      while(tsq < 64)
      {
        whitepassedmask[sq] |= (1ULL << tsq);
        tsq += 8;
      }
      
      tsq = sq - 9;
      while(tsq >= 0) 
      {
        blackpassedmask[sq] |= (1ULL << tsq);
        tsq -= 8;
      }

    }

    if(filesbrd[SQ120(sq)] < FILE_H)
    {
      isolatedmask[sq] |= filebbmask[filesbrd[SQ120(sq)]]

    }

}

void initfilesranksbrd()
{
  int i=0, file = FILE_A, rank = RANK_1, sq=A1, sq64=0;
  for(i=0;i<SQ_NUM;i++)
  {
    filesbrd[i]=OFFBOARD;
    ranksbrd[i]=OFFBOARD;
  }

  for(rank=RANK_1;rank<=RANK_8;rank++)
    for(file=FILE_A;file<=FILE_H;file++)
    {
      sq=FR2SQ(file,rank);
      filesbrd[sq] = file;
      ranksbrd[sq] = rank;
      }
  /*printf("Filesbrd\n");
  for(i=0;i<SQ_NUM;i++)
  {
    if(i%10==0 && i!=0) printf("\n");
    printf("%4d",filesbrd[i]);
  }
  printf("Ranksbrd\n");
  for(i=0;i<SQ_NUM;i++)
  {
    if(i%10==0 && i!=0) printf("\n");
    printf("%4d",ranksbrd[i]);
  }*/
}


void initbitmasks()
{
  int i=0;
  for(i=0;i<64;i++)
  {
    setmask[i] =0ULL;
    clearmask[i] =0ULL;
  }

  for(i=0;i<64;i++)
  {
    setmask[i] |= (1ULL<<i);
    clearmask[i] =~setmask[i];
  }

}

void inithashkeys()
{
  int i=0;
  int i2=0;
  
  sidekey=RAND_64;
  
  for(i=0;i<13;i++)
    for(i2=0;i2<120;i2++)
      piecekeys[i][i2]= RAND_64;
  
  for(i=0;i<16;i++)
    castlekeys[i]=RAND_64;
}


void initsq120to64()
{
  int i=0;
  int file=FILE_A;
  int rank=RANK_1;
  int sq=A1;
  int sq64=0;
  for(i=0;i<SQ_NUM;++i)
    sq120tosq64[i]=65;
  for(i=0;i<64;i++)
    sq64tosq120[i]=120;

  for(rank=RANK_1;rank<=RANK_8;rank++)
    for(file=FILE_A;file<=FILE_H;file++)
    {
      sq=FR2SQ(file,rank);
      sq64tosq120[sq64]=sq;
      sq120tosq64[sq]=sq64;
      sq64++;
    }

}
void Allinit()
{
  initsq120to64();
  initbitmasks();
  inithashkeys();
  initfilesranksbrd();
  initevalmasks();
  InitMvvLva();
}

