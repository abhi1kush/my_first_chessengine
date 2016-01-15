//movegen.c

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
 13 change side, increment ply and hisply
 */

#include "stdio.h"
#include "header.h"

/* movegen(board,list)
 *  Loop all pieces
 *  -> if Slider loop each dir and move
 *    -> Addmove[ list->moves[list->count] = move; list->count++;
*/

#define MOVE(f,t,ca,pro,f1) ((f)|((t)<<7)|((ca)<<14)|((pro)<<20)|(f1))
#define SQOFFBOARD(sq) (filesbrd[(sq)]==OFFBOARD)

int loopslidepce[8]= {wb,wr,wq,0,bb,br,bq,0};
int loopslideindex[2]={0,4};

int loopnonslidepce[6]={wn,wk,0,bn,bk,0};
int loopnonslideindex[2]={0,3};

void addquietmove(const S_BOARD *pos,int move, S_MOVELIST *list)
{
  list->moves[list->count].move = move;
  list->moves[list->count].score = 0;
  list->count++;
}

void addcapturemove(const S_BOARD *pos,int move, S_MOVELIST *list)
{
  list->moves[list->count].move = move;
  list->moves[list->count].score = 0;
  list->count++;
}

void addenpassntmove(const S_BOARD *pos,int move, S_MOVELIST *list)
{
  list->moves[list->count].move = move;
  list->moves[list->count].score = 0;
  list->count++;
}

void addwhitepawncapmove(const S_BOARD *pos, const int from , const int to, const int cap, S_MOVELIST * list)
{
  ASSERT(piecevalidempty(cap));
  ASSERT(sqonboard(from));
  ASSERT(sqonboard(to));

  if(ranksbrd[from]==RANK_7)
  {
    addcapturemove(pos,MOVE(from,to,cap,wq,0),list);
    addcapturemove(pos,MOVE(from,to,cap,wr,0),list);
    addcapturemove(pos,MOVE(from,to,cap,wb,0),list);
    addcapturemove(pos,MOVE(from,to,cap,wn,0),list);
  }
  else
    addcapturemove(pos,MOVE(from,to,cap,EMPTY,0),list);
}

void addwhitepawnmove(const S_BOARD *pos, const int from , const int to, S_MOVELIST * list)
{
  ASSERT(sqonboard(from));
  ASSERT(sqonboard(to));
  if(ranksbrd[from]==RANK_7)
  {
    addcapturemove(pos,MOVE(from,to,EMPTY,wq,0),list);
    addcapturemove(pos,MOVE(from,to,EMPTY,wr,0),list);
    addcapturemove(pos,MOVE(from,to,EMPTY,wb,0),list);
    addcapturemove(pos,MOVE(from,to,EMPTY,wn,0),list);
  }
  else
    addcapturemove(pos,MOVE(from,to,EMPTY,EMPTY,0),list);
}

void addblackpawncapmove(const S_BOARD *pos, const int from , const int to, const int cap, S_MOVELIST * list)
{
  ASSERT(piecevalidempty(cap));
  ASSERT(sqonboard(from));
  ASSERT(sqonboard(to));
  if(ranksbrd[from]==RANK_2)
  {
    addcapturemove(pos,MOVE(from,to,cap,bq,0),list);
    addcapturemove(pos,MOVE(from,to,cap,br,0),list);
    addcapturemove(pos,MOVE(from,to,cap,bb,0),list);
    addcapturemove(pos,MOVE(from,to,cap,bn,0),list);
  }
  else
    addcapturemove(pos,MOVE(from,to,cap,EMPTY,0),list);
}

void addblackpawnmove(const S_BOARD *pos, const int from , const int to, S_MOVELIST * list)
{
  ASSERT(sqonboard(from));
  ASSERT(sqonboard(to));
  if(ranksbrd[from]==RANK_2)
  {
    addcapturemove(pos,MOVE(from,to,EMPTY,bq,0),list);
    addcapturemove(pos,MOVE(from,to,EMPTY,br,0),list);
    addcapturemove(pos,MOVE(from,to,EMPTY,bb,0),list);
    addcapturemove(pos,MOVE(from,to,EMPTY,bn,0),list);
  }
  else
    addcapturemove(pos,MOVE(from,to,EMPTY,EMPTY,0),list);
}

void generateallmoves(const S_BOARD *pos, S_MOVELIST *list)
{
  ASSERT(checkboard(pos));

  list->count = 0;

  int pce = EMPTY;
  int side = pos->side;
  int sq = 0; int t_sq =0;
  int pceNum = 0;
  int dir =0;
  int index =0;
  int pceindex =0;

  printf("\n\n Side: %d\n",side);

  if(side == WHITE)
  {
    for(pceNum =0; pceNum < pos->pceNum[wp];pceNum++)
    {
      sq = pos->plist[wp][pceNum];
      ASSERT(sqonboard(sq));
      if(pos->pieces[sq + 10] == EMPTY)
      {
        addwhitepawnmove(pos,sq,sq+10,list);
        if(ranksbrd[sq]== RANK_2 && pos-> pieces[sq + 20]==EMPTY)
            addquietmove(pos,MOVE(sq,(sq+20),EMPTY,EMPTY,MFLAGPS),list);
      }

      if(!SQOFFBOARD(sq+9) && piececol[pos->pieces[sq+9]]==BLACK)
        addwhitepawncapmove(pos,sq,sq+9,pos->pieces[sq+9],list);
      if(!SQOFFBOARD(sq+11) && piececol[pos->pieces[sq+11]]==BLACK)
        addwhitepawncapmove(pos,sq,sq+11,pos->pieces[sq+11],list);
      //enpassnt
      if(sq+9==pos->enpass)
        addcapturemove(pos,MOVE(sq,sq+9,EMPTY,EMPTY,MFLAGEP),list);
      if(sq+11==pos->enpass)
        addcapturemove(pos,MOVE(sq,sq+11,EMPTY,EMPTY,MFLAGEP),list);
    }

  }
  else
  {
    for(pceNum =0; pceNum < pos->pceNum[bp];pceNum++)
    {
      sq = pos->plist[bp][pceNum];
      ASSERT(sqonboard(sq));
      if(pos->pieces[sq - 10] == EMPTY)
      {
        addblackpawnmove(pos,sq,sq-10,list);
        if(ranksbrd[sq]== RANK_7 && pos-> pieces[sq-20]==EMPTY)
        {
            addquietmove(pos,MOVE(sq,(sq-20),EMPTY,EMPTY,MFLAGPS),list);
        }
      }

      if(!SQOFFBOARD(sq-9) && piececol[pos->pieces[sq-9]]==WHITE)
        addblackpawncapmove(pos,sq,sq-9,pos->pieces[sq-9],list);
      if(!SQOFFBOARD(sq-11) && piececol[pos->pieces[sq-11]]==WHITE)
        addblackpawncapmove(pos,sq,sq-11,pos->pieces[sq-11],list);

      //enpassnt
      if(sq-9==pos->enpass)
        addcapturemove(pos,MOVE(sq,sq-9,EMPTY,EMPTY,MFLAGEP),list);
      if(sq-11==pos->enpass)
        addcapturemove(pos,MOVE(sq,sq-11,EMPTY,EMPTY,MFLAGEP),list);
    }


  }

  /* Loop for slide pieces */
  pceindex = loopslideindex[side];
  pce = loopslidepce[pceindex++];
  while(pce != 0)
  {
    ASSERT(piecevalid(pce));
    printf("slider pceindex:%d pce : %d\n",pceindex,pce);
    pce = loopslidepce[pceindex++];
  }

  /* Loop for slide pieces */
  
  /* Loop for slide pieces */
  pceindex = loopnonslideindex[side];
  pce = loopnonslidepce[pceindex++];
  while(pce != 0)
  {
    ASSERT(piecevalid(pce));
    printf("non slider pceindex:%d pce : %d\n",pceindex,pce);
    pce = loopnonslidepce[pceindex++];
  }

  /* Loop for slide pieces */

}

