//evaluate.c

#include "stdio.h"
#include "header.h"

const int pawnisolated = -10;
const int pawnpassed[8] = {0, 5, 10, 20, 35, 60, 100, 200};
const int rookopenfile = 10;
const int rooksemiopenfile =5;
const int queenopenfile = 5;
const int queensemiopenfile = 3;
const int bishoppair = 30; 

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

const int KingE[64] = { 

  -50 , -10 , 0 , 0 , 0 , 0 , -10 , -50 ,

  -10,  0 , 10  , 10  , 10  , 10  , 0 , -10 ,

  0 , 10  , 15  , 15  , 15  , 15  , 10  , 0 ,

  0 , 10  , 15  , 20  , 20  , 15  , 10  , 0 ,

  0 , 10  , 15  , 20  , 20  , 15  , 10  , 0 ,

  0 , 10  , 15  , 15  , 15  , 15  , 10  , 0 ,

  -10,  0 , 10  , 10  , 10  , 10  , 0 , -10 ,

  -50 , -10 , 0 , 0 , 0 , 0 , -10 , -50 

};



const int KingO[64] = { 

  0 , 5 , 5 , -10 , -10 , 0 , 10  , 5 ,

  -30 , -30 , -30 , -30 , -30 , -30 , -30 , -30 ,

  -50 , -50 , -50 , -50 , -50 , -50 , -50 , -50 ,

  -70 , -70 , -70 , -70 , -70 , -70 , -70 , -70 ,

  -70 , -70 , -70 , -70 , -70 , -70 , -70 , -70 ,

  -70 , -70 , -70 , -70 , -70 , -70 , -70 , -70 ,

  -70 , -70 , -70 , -70 , -70 , -70 , -70 , -70 ,

  -70 , -70 , -70 , -70 , -70 , -70 , -70 , -70   

};

// sjeng 11.2
//8/6R1/2k5/6P1/8/8/4nP2/6K1 w - - 1 41 
int materialdraw(const S_BOARD *pos) 
{
    if (!pos->pceNum[wr] && !pos->pceNum[br] && !pos->pceNum[wq] && !pos->pceNum[bq]) 
    {
      if (!pos->pceNum[bb] && !pos->pceNum[wb]) 
      {
        if (pos->pceNum[wn] < 3 && pos->pceNum[bn] < 3) {  return TRUE; }
      } 
      else if (!pos->pceNum[wn] && !pos->pceNum[bn]) 
      {
        if (abs(pos->pceNum[wb] - pos->pceNum[bb]) < 2) { return TRUE; }
      } 
      else if ((pos->pceNum[wn] < 3 && !pos->pceNum[wb]) || (pos->pceNum[wb] == 1 && !pos->pceNum[wn])) 
      {
        if ((pos->pceNum[bn] < 3 && !pos->pceNum[bb]) || (pos->pceNum[bb] == 1 && !pos->pceNum[bn]))  { return TRUE; }
      }
    } 
    else if (!pos->pceNum[wq] && !pos->pceNum[bq]) 
    {
      if (pos->pceNum[wr] == 1 && pos->pceNum[br] == 1) 
      {
        if ((pos->pceNum[wn] + pos->pceNum[wb]) < 2 && (pos->pceNum[bn] + pos->pceNum[bb]) < 2)	{ return TRUE; }
      } 
      else if (pos->pceNum[wr] == 1 && !pos->pceNum[br]) 
      {
        if ((pos->pceNum[wn] + pos->pceNum[wb] == 0) && (((pos->pceNum[bn] + pos->pceNum[bb]) == 1) || ((pos->pceNum[bn] + pos->pceNum[bb]) == 2))) { return TRUE; }
      } 
      else if (pos->pceNum[br] == 1 && !pos->pceNum[wr]) 
      {
        if ((pos->pceNum[bn] + pos->pceNum[bb] == 0) && (((pos->pceNum[wn] + pos->pceNum[wb]) == 1) || ((pos->pceNum[wn] + pos->pceNum[wb]) == 2))) { return TRUE; }
      }
    }
    return FALSE;
}

#define ENDGAME_MAT (1* pieceval[wr] + 2*pieceval[wn] + 3*pieceval[wp])

int evalposition(const S_BOARD *pos)
{
  int pce;
  int pcenum;
  int sq;
  int score = pos->material[WHITE] - pos->material[BLACK];
 
  if(!pos->pceNum[wp] && !pos->pceNum[bp] && materialdraw(pos) == TRUE)
    return 0;


  pce = wp;
  for(pcenum =0; pcenum < pos->pceNum[pce]; pcenum++)
  {
    sq = pos->plist[pce][pcenum];
    ASSERT(sqonboard(sq));
    score += pawntable[SQ64(sq)];

    if((isolatedmask[SQ64(sq)] & pos->pawns[WHITE])==0)
    {  
      //printf("wp Iso:%s\n",prsq(sq));
      score += pawnisolated;
    }
     
    if((whitepassedmask[SQ64(sq)] & pos->pawns[BLACK])==0)
     {
       //printf("bp passed: %s\n",prsq(sq));
       score += pawnpassed[ranksbrd[sq]];
     }
  }
  
  pce = bp;
  for(pcenum =0; pcenum < pos->pceNum[pce]; pcenum++)
  {
    sq = pos->plist[pce][pcenum];
    ASSERT(sqonboard(sq));
    score -= pawntable[mirror64[SQ64(sq)]];
    
    if((isolatedmask[SQ64(sq)] & pos->pawns[BLACK])==0)
    {
      //printf("bp Iso:%s\n",prsq(sq));
      score -= pawnisolated;
    }
      
     if((blackpassedmask[SQ64(sq)] & pos->pawns[WHITE])==0)
     {
       //printf("bp passed: %s\n",prsq(sq));
       score -= pawnpassed[7-ranksbrd[sq]];
     }

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

    if(!(pos->pawns[BOTH] & filebbmask[filesbrd[sq]]))
      score += rookopenfile;
    else if(!(pos->pawns[WHITE] & filebbmask[filesbrd[sq]]))
      score +=rooksemiopenfile;
  }
  
  
  pce = br;
  for(pcenum =0; pcenum < pos->pceNum[pce]; pcenum++)
  {
    sq = pos->plist[pce][pcenum];
    ASSERT(sqonboard(sq));
    score -= rooktable[mirror64[SQ64(sq)]];
    
    if(!(pos->pawns[BOTH] & filebbmask[filesbrd[sq]]))
      score -= rookopenfile;
    else if(!(pos->pawns[BLACK] & filebbmask[filesbrd[sq]]))
      score -=rooksemiopenfile;
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
  
  pce =wq;
  for(pcenum=0;pcenum < pos->pceNum[pce];pcenum++)
  {
    sq= pos->plist[pce][pcenum];
    ASSERT(sqonboard(sq));
    if(!(pos->pawns[BOTH] & filebbmask[filesbrd[sq]]))
      score += queenopenfile;
    else if(!(pos->pawns[WHITE] & filebbmask[filesbrd[sq]]))
      score += queensemiopenfile;
  }

  pce =bq;
  for(pcenum=0;pcenum < pos->pceNum[pce];pcenum++)
  {
    sq= pos->plist[pce][pcenum];
    ASSERT(sqonboard(sq));
    if(!(pos->pawns[BOTH] & filebbmask[filesbrd[sq]]))
      score -= queenopenfile;
    else if(!(pos->pawns[BLACK] & filebbmask[filesbrd[sq]]))
      score -= queensemiopenfile;
  }
  
  pce =wk;
  sq= pos->plist[pce][0];
  
  if((pos->material[BLACK] <= ENDGAME_MAT))
    score += KingE[SQ64(sq)];
  else
    score += KingO[SQ64(sq)];
  
  pce =bk;
  sq = pos->plist[pce][0];
  
  if((pos->material[WHITE] <= ENDGAME_MAT))
    score -= KingE[SQ64(sq)];
  else
    score -= KingO[SQ64(sq)];

  if(pos->pceNum[wb] >= 2) score += bishoppair;
  if(pos->pceNum[bb] >= 2) score -= bishoppair;
   
  if(pos->side == WHITE)
    return score;
  else
    return -score;

  return 0;
}
