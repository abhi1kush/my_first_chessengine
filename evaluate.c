//evaluate.c

#include "stdio.h"
#include "header.h"

const int pawntable[64] = {
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	,
10	,	10	,	0	,	-10	,	-10	,	0	,	10	,	10	,
5	,	0	,	0	,	5	,	5	,	0	,	0	,	5	,
0	,	0	,	10	,	20	,	20	,	10	,	0	,	0	,
5	,	5	,	5	,	10	,	10	,	5	,	5	,	5	,
10	,	10	,	10	,	20	,	20	,	10	,	10	,	10	,
20	,	20	,	20	,	30	,	30	,	20	,	20	,	20	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

const int knighttable[64] = {
0	,	-10	,	0	,	0	,	0	,	0	,	-10	,	0	,
0	,	0	,	0	,	5	,	5	,	0	,	0	,	0	,
0	,	0	,	10	,	10	,	10	,	10	,	0	,	0	,
0	,	0	,	10	,	20	,	20	,	10	,	5	,	0	,
5	,	10	,	15	,	20	,	20	,	15	,	10	,	5	,
5	,	10	,	10	,	20	,	20	,	10	,	10	,	5	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0		
};

const int bishoptable[64] = {
0	,	0	,	-10	,	0	,	0	,	-10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	10	,	15	,	20	,	20	,	15	,	10	,	0	,
0	,	0	,	10	,	15	,	15	,	10	,	0	,	0	,
0	,	0	,	0	,	10	,	10	,	0	,	0	,	0	,
0	,	0	,	0	,	0	,	0	,	0	,	0	,	0	
};

const int rooktable[64] = {
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0	,
25	,	25	,	25	,	25	,	25	,	25	,	25	,	25	,
0	,	0	,	5	,	10	,	10	,	5	,	0	,	0		
};

const int mirror64[64] = {
56	,	57	,	58	,	59	,	60	,	61	,	62	,	63	,
48	,	49	,	50	,	51	,	52	,	53	,	54	,	55	,
40	,	41	,	42	,	43	,	44	,	45	,	46	,	47	,
32	,	33	,	34	,	35	,	36	,	37	,	38	,	39	,
24	,	25	,	26	,	27	,	28	,	29	,	30	,	31	,
16	,	17	,	18	,	19	,	20	,	21	,	22	,	23	,
8	,	9	,	10	,	11	,	12	,	13	,	14	,	15	,
0	,	1	,	2	,	3	,	4	,	5	,	6	,	7
};


int evalposition(const S_BOARD *pos)
{
  int pce;
  int pcenum;
  int sq;
  int score = pos->material[WHITE] - pos->material[BLACK];
  
  pce = wp;
  for(pcenum =0; pcenum < pos->pceNum[pce]; pcenum++)
  {
    sq = pos->plist[pce][pcenum];
    ASSERT(sqonboard(sq));
    score += pawntable[SQ64(sq)];
  }
  
  pce = bp;
  for(pcenum =0; pcenum < pos->pceNum[pce]; pcenum++)
  {
    sq = pos->plist[pce][pcenum];
    ASSERT(sqonboard(sq));
    score -= pawntable[mirror64[SQ64(sq)]];
  }
  
  pce = wn;
  for(pcenum =0; pcenum < pos->pceNum[pce]; pcenum++)
  {
    sq = pos->plist[pce][pcenum];
    ASSERT(sqonboard(sq));
    score += knighttable[SQ64(sq)];
  }
  
  pce = bn;
  for(pcenum =0; pcenum < pos->pceNum[pce]; pcenum++)
  {
    sq = pos->plist[pce][pcenum];
    ASSERT(sqonboard(sq));
    score -= knighttable[mirror64[SQ64(sq)]];
  }
  
  pce = wr;
  for(pcenum =0; pcenum < pos->pceNum[pce]; pcenum++)
  {
    sq = pos->plist[pce][pcenum];
    ASSERT(sqonboard(sq));
    score += rooktable[SQ64(sq)];
  }
  
  
  pce = br;
  for(pcenum =0; pcenum < pos->pceNum[pce]; pcenum++)
  {
    sq = pos->plist[pce][pcenum];
    ASSERT(sqonboard(sq));
    score -= rooktable[mirror64[SQ64(sq)]];
  }
  
  pce = wb;
  for(pcenum =0; pcenum < pos->pceNum[pce]; pcenum++)
  {
    sq = pos->plist[pce][pcenum];
    ASSERT(sqonboard(sq));
    score += bishoptable[SQ64(sq)];
  }
  
  
  pce = bb;
  for(pcenum =0; pcenum < pos->pceNum[pce]; pcenum++)
  {
    sq = pos->plist[pce][pcenum];
    ASSERT(sqonboard(sq));
    score -= bishoptable[mirror64[SQ64(sq)]];
  }
   
  if(pos->side == WHITE)
    return score;
  else
    return -score;
  return 0;
}
