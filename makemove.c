//makemove.c 

/* Making A Move
 1. make (move)
 2. Get the from , to, cap from the move
 3. store the current position in the pos->history array
 4. move the current piece from -> to
 5. if a capture was made, remove captured  from  the piece list
 6. Update fifty move rule , see if pawn was moved or captured
 7. Promotions
 8. Enpassnt Captures
 9. Set Enpas Squre if Pstart
 10.For all pieces added, moved removed,update position counters, and piece lists.
 11. maintain poskey
 12. don't forget castle permission
 13 change side, increment ply and historyply
 */

#include "stdio.h"
#include "header.h"

#define HASH_PCE(pce,sq) (pos->poskey ^= (piecekeys[(pce)][(sq)]))
#define HASH_CA (pos->poskey ^= (castlekeys[(pos->castle)]))
#define HASH_SIDE (pos->poskey ^= (sidekey))
#define HASH_EP (pos->poskey ^= (piecekeys[EMPTY][(pos->enpass)]))
#define INCHECK (sqattacked(pos->kingsq[pos->side],pos->side^1,pos))
// ca_perm  &= castleperm[from]

const int castleperm[120] = {
15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,
15,13,15,15,15,12,15,15,14,15,
15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,
15,7,15,15,15,3,15,15,11,15,
15,15,15,15,15,15,15,15,15,15,
15,15,15,15,15,15,15,15,15,15
};

void makenullmove(S_BOARD *pos)
{
  ASSERT(checkboard(pos));
  ASSERT(!INCHECK);

  pos->ply++;
  pos->history[pos->historyply].poskey = pos->poskey;

  if(pos->enpass != NO_SQ) HASH_EP;

  pos->history[pos->historyply].move = NOMOVE;
  pos->history[pos->historyply].fiftymove = pos->fiftymove;
  pos->history[pos->historyply].enpass = pos->enpass;
  pos->history[pos->historyply].castle = pos->castle;
  pos->enpass = NO_SQ;

  pos->side ^= 1;
  pos->historyply++;
  HASH_SIDE;

  ASSERT(checkboard(pos));
  return;
}//MakeNullMove

void takenullmove(S_BOARD *pos)
{
  ASSERT(checkboard(pos));

  pos->historyply--;
  pos->ply--;

  if(pos->enpass != NO_SQ) HASH_EP;

  pos->castle = pos->history[pos->historyply].castle;
  pos->fiftymove = pos->history[pos->historyply].fiftymove;
  pos->enpass = pos->history[pos->historyply].enpass;

  if(pos->enpass != NO_SQ) HASH_EP;

  pos->side ^= 1;
  HASH_SIDE;

  ASSERT(checkboard(pos));
}

static void clearpiece(const int sq, S_BOARD *pos)
{
  ASSERT(sqonboard(sq));
  int pce = pos->pieces[sq];
  ASSERT(piecevalid(pce));
  
  int col = piececol[pce];
  int i=0;
  int t_pceNum = -1;

  HASH_PCE(pce,sq);

  pos->pieces[sq] = EMPTY;
  pos->material[col] -= pieceval[pce];

  if(piecebig[pce])
  {
    pos->bigpce[col]--;
    if(piecemaj[pce])
      pos->majpce[col]--;
    else
      pos->minpce[col]--;
  }
  else
  {
    clrbit(pos->pawns[col],SQ64(sq));
    clrbit(pos->pawns[BOTH],SQ64(sq));
  }
  /*
   *  pos->pceNum[wp] == 5 looping from 0 to 4
   *  pos->plist[pce][0] == sq0
   *  pos->plist[pce][1] == sq1
   *  pos->plist[pce][2] == sq2
   *  pos->plist[pce][3] == sq3
   *  pos->plist[pce][4] == sq4
   *
   *  sq == sq3 so t_pceNum = 3;
   */
  for(i=0;i< pos->pceNum[pce];i++)
  {
    if(pos->plist[pce][i] == sq)
    {
      t_pceNum =i;
      break;
    }
  }
  ASSERT(t_pceNum != -1);
  
  pos->pceNum[pce]--;
  //pos->pceNum[wp] == 4

  pos->plist[pce][t_pceNum] = pos->plist[pce][pos->pceNum[pce]];
  /*pos->plist[wp][3] = pos->plist[wp][4] <= sq4
   *
   * pos->pceNum[wp] == 4 looping from 0 to 3
   * pos->plist[pce][0] == sq0
   * pos->plist[pce][1] == sq1
   * pos->plist[pce][2] == sq2
   * pos->plist[pce][3] == sq4
  */

}

static void addpiece(const int sq, S_BOARD *pos,const int pce)
{
  ASSERT(piecevalid(pce));
  ASSERT(sqonboard(sq));

  int col = piececol[pce];

  HASH_PCE(pce,sq);

  pos->pieces[sq] = pce;

  if(piecebig[pce])
  {
    pos->bigpce[col]++;
    if(piecemaj[pce])
      pos->majpce[col]++;
    else
      pos->minpce[col]++;
  }
  else
  {
    setbit(pos->pawns[col],SQ64(sq));
    setbit(pos->pawns[BOTH],SQ64(sq));
  }
  pos->material[col] += pieceval[pce];
  pos->plist[pce][pos->pceNum[pce]++] = sq;
}

static void movepiece(const int from,const int to, S_BOARD *pos)
{
  ASSERT(sqonboard(from));
  ASSERT(sqonboard(to));

  int i=0;
  int pce = pos->pieces[from];
  int col = piececol[pce];

#ifdef DEBUG
  int t_pieceNum =FALSE;
#endif
  HASH_PCE(pce,from);
  pos->pieces[from] = EMPTY;

  HASH_PCE(pce,to);
  pos->pieces[to] = pce;

  if(!piecebig[pce])
  {
    clrbit(pos->pawns[col],SQ64(from));
    clrbit(pos->pawns[BOTH],SQ64(from));
    setbit(pos->pawns[col],SQ64(to));
    setbit(pos->pawns[BOTH],SQ64(to));

  }
  
  for(i=0;i<pos->pceNum[pce];i++)
  {
    if(pos->plist[pce][i] == from)
    {
      pos->plist[pce][i] = to;
#ifdef DEBUG
      t_pieceNum =TRUE;
#endif
      break;
    }
  }
  ASSERT(t_pieceNum);
}

int makemove(S_BOARD *pos,int move)
{
  //printf("makemove assert checkboard");
  ASSERT(checkboard(pos));
int from = FROMSQ(move);
int to = TOSQ(move);
int side = pos->side;

ASSERT(sqonboard(from));
ASSERT(sqonboard(to));
ASSERT(sidevalid(side));
ASSERT(piecevalid(pos->pieces[from]));

pos->history[pos->historyply].poskey = pos->poskey;

if(move & MFLAGEP)
{
  if(side==WHITE)
    clearpiece(to-10,pos);
  else
    clearpiece(to+10,pos);
}
else if(move & MFLAGCA)
{
  switch(to)
  {
    case C1: 
      movepiece(A1,D1,pos);
      break;
    case C8:
      movepiece(A8,D8,pos);
      break;
    case G1:
      movepiece(H1,F1,pos);
      break;
    case G8:
      movepiece(H8,F8,pos);
      break;
    default : ASSERT(FALSE);
  }
}

if(pos->enpass != NO_SQ) 
  HASH_EP;
HASH_CA;

pos->history[pos->historyply].move = move;
pos->history[pos->historyply].fiftymove = pos->fiftymove;
pos->history[pos->historyply].enpass = pos->enpass;
pos->history[pos->historyply].castle = pos->castle;

pos->castle &= castleperm[from];
pos->castle &= castleperm[to];
pos->enpass = NO_SQ;

HASH_CA;

int captured = CAPTURED(move);
pos->fiftymove++;

if(captured != EMPTY)
{
  ASSERT(piecevalid(captured));
  clearpiece(to,pos);
  pos->fiftymove =0;
}

pos->historyply++;
pos->ply++;

if(piecepawn[pos->pieces[from]])
{
  pos->fiftymove=0;
  if(move & MFLAGPS)
  {
    if(side==WHITE)
    {
      pos->enpass = from +10;
      ASSERT(ranksbrd[pos->enpass]==RANK_3);
    }
    else
    {
      pos->enpass = from -10;
      ASSERT(ranksbrd[pos->enpass]==RANK_6);
    }
    HASH_EP;
  }
}

movepiece(from,to,pos);

int prompce = PROMOTED(move);
if(prompce != EMPTY)
{
  ASSERT(piecevalid(prompce) && !piecepawn[prompce]);
  clearpiece(to,pos);
  addpiece(to,pos,prompce);
}
if(pieceking[pos->pieces[to]])
  pos->kingsq[pos->side] = to;

  pos->side ^= 1;
  HASH_SIDE;
  //printf("checkboard in movepiece");
  ASSERT(checkboard(pos));

if(sqattacked(pos->kingsq[side],pos->side,pos))
{ 
  //king in check still after move
  //printf("king is still in check");
  takemove(pos);
  return FALSE;
}

return TRUE;

}

void takemove(S_BOARD *pos )
{
  //:printf("checkboard in takemove");
  ASSERT(checkboard(pos));

  pos->historyply--;
  pos->ply--;

  int move = pos->history[pos->historyply].move;
  int from = FROMSQ(move);
  int to = TOSQ(move);

  ASSERT(sqonboard(from));
  ASSERT(sqonboard(to));
  
  //printf("second last checkboard in makemove.c\n current ***** poskey %llx\n",pos->poskey);
  ASSERT(checkboard(pos));

  if(pos->enpass != NO_SQ)
  {
    HASH_EP;
    //printf("after hashep current ***** poskey %llx\n",pos->poskey);
  }
  HASH_CA;
  //printf("after hashca current ***** poskey %llx\n",pos->poskey);

  pos->castle = pos->history[pos->historyply].castle;
  pos->fiftymove = pos->history[pos->historyply].fiftymove;
  pos->enpass = pos->history[pos->historyply].enpass;
  
  if(pos->enpass != NO_SQ)
  {
    HASH_EP;
    //printf("after hashep current ***** poskey %llx\n",pos->poskey);
  }
  HASH_CA;
  //printf("after hashca current ***** poskey %llx\n",pos->poskey);

  
  pos->side ^=1;
  
  HASH_SIDE;
  //printf("after hash_side current ***** poskey %llx\n",pos->poskey);

  if(MFLAGEP & move)
  {
    if(pos->side == WHITE)
      addpiece(to-10,pos,bp);
    else
      addpiece(to+10,pos,wp);
  }
  else if(MFLAGCA & move)
  {
    switch(to)
    {
      case C1 : movepiece(D1,A1,pos);break;
      case C8: movepiece(D8,A8,pos);break;
      case G1: movepiece(F1,H1,pos);break;
      case G8: movepiece(F8,H8,pos);break;
      default : ASSERT(FALSE); 
    }
  }
  //printf("line 342 current ***** poskey %llx\n",pos->poskey);
  movepiece(to,from,pos);
  if(pieceking[pos->pieces[from]])
    pos->kingsq[pos->side] = from;
  
  //printf("line 347 current ***** poskey %llx\n",pos->poskey);

  int captured = CAPTURED(move);
  if (captured != EMPTY)
  {
    ASSERT(piecevalid(captured));
    addpiece(to,pos,captured);
  }
  
  if(PROMOTED(move) != EMPTY)
  {
    ASSERT(piecevalid(PROMOTED(move)) && !piecepawn[PROMOTED(move)]);
    clearpiece(from,pos);
    addpiece(from,pos,(piececol[PROMOTED(move)] == WHITE ? wp : bp));
  }
  //printf("last checkboard in makemove.c\n current ***** poskey %llx\n",pos->poskey);
  ASSERT(checkboard(pos));

}
