#include "header.h"

int sq120tosq64[SQ_NUM];
int sq64tosq120[64];

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
  
}

