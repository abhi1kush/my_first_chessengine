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

const int loopslidepce[8]= {wb,wr,wq,0,bb,br,bq,0};
const int loopslideindex[2]={0,4};

const int loopnonslidepce[6]={wn,wk,0,bn,bk,0};
const int loopnonslideindex[2]={0,3};

const int pcedir[13][8] = {
  { 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0 },
  { -8, -19,  -21, -12, 8, 19, 21, 12 },
  { -9, -11, 11, 9, 0, 0, 0, 0 },
  { -1, -10,  1, 10, 0, 0, 0, 0 },
  { -1, -10,  1, 10, -9, -11, 11, 9 },
  { -1, -10,  1, 10, -9, -11, 11, 9 },
  { 0, 0, 0, 0, 0, 0, 0 },
  { -8, -19,  -21, -12, 8, 19, 21, 12 },
  { -9, -11, 11, 9, 0, 0, 0, 0 },
  { -1, -10,  1, 10, 0, 0, 0, 0 },
  { -1, -10,  1, 10, -9, -11, 11, 9 },
  { -1, -10,  1, 10, -9, -11, 11, 9 }
};

const int numdir[13] = {0,0,8,4,4,8,8,0,8,4,4,8,8};
/*
 * pv move
 * cap -> MvvLVA (SEE)
 * Killers (beta cutoff)
 * HistoryScore (inc alpha)
 *
 * [vic][att]
 *  p x Q
 *  N x Q
 *  B x Q
 *  R x Q
 *  ...
 *  p x R
 *  N x R
 *  
 *  B
 *
 *  N
 *
 *  p
 *
 *  vic Q -> 500 p(505) N(504) ...
 *  vic R -> 400 p(405) N(404) ...
 */

const int victimscore[13] = {0,100,200,300,400,500,600,100,200,300,400,500,600};
static int MvvLvaScores[13][13];

int InitMvvLva()
{
  int attacker;
  int victim;
  for(attacker = wp; attacker <= bk; attacker++)
    for(victim = wp;victim <= bk; victim++)
      MvvLvaScores[victim][attacker] = victimscore[victim] + 6 - (victimscore[attacker]/100);
  
 /* for(victim = wp;victim <= bk; victim++)
    for(attacker = wp; attacker <= bk; attacker++)
      printf("%c x %c = %d\n",pcechar[attacker],pcechar[victim],MvvLvaScores[victim][attacker]);
  */
  return 0;
}


static void addquietmove(const S_BOARD *pos,int move, S_MOVELIST *list)
{
  ASSERT(sqonboard(FROMSQ(move)));
  ASSERT(sqonboard(TOSQ(move)));

  list->moves[list->count].move = move;
  
 if(pos->searchkillers[0][pos->ply]==move)
 {
  list->moves[list->count].score = 900000; 
 }
 else if(pos->searchkillers[1][pos->ply] == move)
 {
  list->moves[list->count].score = 800000; 
 }
 else
 {
  list->moves[list->count].score = pos->searchhistory[pos->pieces[FROMSQ(move)]][TOSQ(move)]; 
 }
  list->count++;
}

static void addcapturemove(const S_BOARD *pos,int move, S_MOVELIST *list)
{
  ASSERT(sqonboard(FROMSQ(move)));
  ASSERT(sqonboard(TOSQ(move)));
  ASSERT(piecevalid(CAPTURED(move)));
  
  list->moves[list->count].move = move;
  list->moves[list->count].score = MvvLvaScores[CAPTURED(move)][pos->pieces[FROMSQ(move)]]+1000000;
  list->count++;
}

static void addenpassantmove(const S_BOARD *pos,int move, S_MOVELIST *list)
{
  ASSERT(sqonboard(FROMSQ(move)));
  ASSERT(sqonboard(TOSQ(move)));
  
  list->moves[list->count].move = move;
  list->moves[list->count].score = 105+1000000;
  list->count++;
}

static void addwhitepawncapmove(const S_BOARD *pos, const int from , const int to, const int cap, S_MOVELIST * list)
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

static void addwhitepawnmove(const S_BOARD *pos, const int from , const int to, S_MOVELIST * list)
{
  ASSERT(sqonboard(from));
  ASSERT(sqonboard(to));
  if(ranksbrd[from]==RANK_7)
  {
    addquietmove(pos,MOVE(from,to,EMPTY,wq,0),list);
    addquietmove(pos,MOVE(from,to,EMPTY,wr,0),list);
    addquietmove(pos,MOVE(from,to,EMPTY,wb,0),list);
    addquietmove(pos,MOVE(from,to,EMPTY,wn,0),list);
  }
  else
    addquietmove(pos,MOVE(from,to,EMPTY,EMPTY,0),list);
}

static void addblackpawncapmove(const S_BOARD *pos, const int from , const int to, const int cap, S_MOVELIST * list)
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

static void addblackpawnmove(const S_BOARD *pos, const int from , const int to, S_MOVELIST * list)
{
  ASSERT(sqonboard(from));
  ASSERT(sqonboard(to));
  if(ranksbrd[from]==RANK_2)
  {
    addquietmove(pos,MOVE(from,to,EMPTY,bq,0),list);
    addquietmove(pos,MOVE(from,to,EMPTY,br,0),list);
    addquietmove(pos,MOVE(from,to,EMPTY,bb,0),list);
    addquietmove(pos,MOVE(from,to,EMPTY,bn,0),list);
  }
  else
    addquietmove(pos,MOVE(from,to,EMPTY,EMPTY,0),list);
}

void generateallmoves(const S_BOARD *pos, S_MOVELIST *list)
{
  //printf("generate checkboard assert");
  ASSERT(checkboard(pos));

  list->count = 0;

  int pce = EMPTY;
  int side = pos->side;
  int sq = 0; int t_sq =0;
  int pceNum = 0;
  int dir =0;
  int i =0;
  int pceindex =0;

  //printf("\n\n Side: %d\n",side);

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
      if(!SQOFFBOARD(sq+9) && sq+9==pos->enpass)
        addenpassantmove(pos,MOVE(sq,sq+9,EMPTY,EMPTY,MFLAGEP),list);
      if(!SQOFFBOARD(sq+11) && sq+11==pos->enpass)
        addenpassantmove(pos,MOVE(sq,sq+11,EMPTY,EMPTY,MFLAGEP),list);
    } //for ended

    /* castle move */
    if(pos->castle & WO_O)
    {
      if(pos->pieces[F1]==EMPTY && pos->pieces[G1] == EMPTY)
        if(!sqattacked(E1,BLACK,pos) && !sqattacked(F1,BLACK,pos) && !sqattacked(G1,BLACK,pos))
        {
          //printf("WO_O MoveGen\n");
          addquietmove(pos,MOVE(E1,G1,EMPTY,EMPTY,MFLAGCA),list);
        }

    }
    if(pos->castle & WO_O_O)
    {
      if(pos->pieces[D1]==EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[B1]==EMPTY)
        if(!sqattacked(E1,BLACK,pos) && !sqattacked(D1,BLACK,pos) && !sqattacked(C1,BLACK,pos))
        {
          //printf("WO_O_O MoveGen\n");
          addquietmove(pos,MOVE(E1,C1,EMPTY,EMPTY,MFLAGCA),list);
        }

    }
    /* castle move */

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
      if(!SQOFFBOARD(sq-9) && sq-9==pos->enpass)
        addenpassantmove(pos,MOVE(sq,sq-9,EMPTY,EMPTY,MFLAGEP),list);
      if(!SQOFFBOARD(sq-11) && sq-11==pos->enpass)
        addenpassantmove(pos,MOVE(sq,sq-11,EMPTY,EMPTY,MFLAGEP),list);
    }
    
    /* castle move */
    if(pos->castle & BO_O)
    {
      if(pos->pieces[F8]==EMPTY && pos->pieces[G8] == EMPTY)
        if(!sqattacked(E8,WHITE,pos) && !sqattacked(F8,WHITE,pos) && !sqattacked(G8,WHITE,pos))
        {
          //printf("BO_O MoveGen\n");
          addquietmove(pos,MOVE(E8,G8,EMPTY,EMPTY,MFLAGCA),list);
        }

    }
    if(pos->castle & BO_O_O)
    {
      if(pos->pieces[D8]==EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[B8]==EMPTY)
        if(!sqattacked(E8,WHITE,pos) && !sqattacked(D8,WHITE,pos) && !sqattacked(C8,WHITE,pos))
        {
          //printf("BO_O_O MoveGen\n");
          addquietmove(pos,MOVE(E8,C8,EMPTY,EMPTY,MFLAGCA),list);
        }

    }
    /* castle move */


  }

  /* Loop for slide pieces */  
  
  pceindex = loopslideindex[side];
  pce = loopslidepce[pceindex++];
  
  while(pce != 0)
  {
    ASSERT(piecevalid(pce));
    //printf("slider pceindex:%d pce : %d\n",pceindex,pce);
    
    for(pceNum =0; pceNum < pos->pceNum[pce];pceNum++)
    {
      sq=pos->plist[pce][pceNum];
      ASSERT(sqonboard(sq));
      //printf("Piece : %c on %s\n",pcechar[pce],prsq(sq));
      
      for(i=0;i<numdir[pce];i++)
      {
        dir = pcedir[pce][i];
        t_sq = sq +dir;
        
        while(!SQOFFBOARD(t_sq))
        {
          if(pos->pieces[t_sq] != EMPTY)
          {
            if(piececol[pos->pieces[t_sq]]== side^1)
            {//printf("\t\tCapture on %s \n", prsq(t_sq));
             addcapturemove(pos,MOVE(sq,t_sq,pos->pieces[t_sq],EMPTY,0),list);
            }
            break;
          }
          //printf("\t\tNormal on %s\n",prsq(t_sq));
          addquietmove(pos,MOVE(sq,t_sq,EMPTY,EMPTY,0),list);
          t_sq += dir;
        }
      }
    }
    pce = loopslidepce[pceindex++];
  }
  /* Loop for slide pieces */
  
  /* Loop for non slide pieces */
  
  pceindex = loopnonslideindex[side];
  pce = loopnonslidepce[pceindex++];
  
  while(pce != 0)
  {
    ASSERT(piecevalid(pce));
    //printf("non slider pceindex:%d pce : %d\n",pceindex,pce);
    
    for(pceNum =0; pceNum <pos->pceNum[pce];pceNum++)
    {
      sq=pos->plist[pce][pceNum];
      ASSERT(sqonboard(sq));
      //printf("Piece : %c on %s\n",pcechar[pce],prsq(sq));
      for(i=0;i<numdir[pce];i++)
      {
        dir = pcedir[pce][i];
        t_sq = sq +dir;

        if(SQOFFBOARD(t_sq))
          continue;
        // BLACK ^ 1 == WHITE      WHITE ^ 1 == BLACK
        if(pos->pieces[t_sq] != EMPTY)
        {
          if(piececol[pos->pieces[t_sq]] == side ^ 1)
          {//printf("\t\tCapture on %s\n",prsq(t_sq));
            addcapturemove(pos,MOVE(sq,t_sq,pos->pieces[t_sq],EMPTY,0),list);}
          continue;
        }
        //printf("\t\tNormal on %s \n",prsq(t_sq));
        addquietmove(pos,MOVE(sq,t_sq,EMPTY,EMPTY,0),list);
      }
    }
    pce = loopnonslidepce[pceindex++];
  }

  /* Loop for non slide pieces */

}

int moveexists(S_BOARD *pos,const int move)
{
  S_MOVELIST list[1];
  generateallmoves(pos,list);

  int movenum = 0;
  for(movenum = 0; movenum < list->count; movenum++)
  {
    if(!makemove(pos,list->moves[movenum].move))
      continue;
    takemove(pos);
    if(list->moves[movenum].move == move)
      return TRUE; 
  }
  return FALSE;

}

void generateallcaps(const S_BOARD *pos, S_MOVELIST *list)
{
  //printf("generate checkboard assert");
  ASSERT(checkboard(pos));

  list->count = 0;

  int pce = EMPTY;
  int side = pos->side;
  int sq = 0; int t_sq =0;
  int pceNum = 0;
  int dir =0;
  int i =0;
  int pceindex =0;

  //printf("\n\n Side: %d\n",side);

  if(side == WHITE)
  {
    for(pceNum =0; pceNum < pos->pceNum[wp];pceNum++)
    {
      sq = pos->plist[wp][pceNum];
      ASSERT(sqonboard(sq));

      if(!SQOFFBOARD(sq+9) && piececol[pos->pieces[sq+9]]==BLACK)
        addwhitepawncapmove(pos,sq,sq+9,pos->pieces[sq+9],list);
      if(!SQOFFBOARD(sq+11) && piececol[pos->pieces[sq+11]]==BLACK)
        addwhitepawncapmove(pos,sq,sq+11,pos->pieces[sq+11],list);
      //enpassnt
      if(!SQOFFBOARD(sq+9) && sq+9==pos->enpass)
        addenpassantmove(pos,MOVE(sq,sq+9,EMPTY,EMPTY,MFLAGEP),list);
      if(!SQOFFBOARD(sq+11) && sq+11==pos->enpass)
        addenpassantmove(pos,MOVE(sq,sq+11,EMPTY,EMPTY,MFLAGEP),list);
    } //for ended


  }
  else
  {
    for(pceNum =0; pceNum < pos->pceNum[bp];pceNum++)
    {
      sq = pos->plist[bp][pceNum];
      ASSERT(sqonboard(sq));

      if(!SQOFFBOARD(sq-9) && piececol[pos->pieces[sq-9]]==WHITE)
        addblackpawncapmove(pos,sq,sq-9,pos->pieces[sq-9],list);
      if(!SQOFFBOARD(sq-11) && piececol[pos->pieces[sq-11]]==WHITE)
        addblackpawncapmove(pos,sq,sq-11,pos->pieces[sq-11],list);

      //enpassnt
      if(!SQOFFBOARD(sq-9) && sq-9==pos->enpass)
        addenpassantmove(pos,MOVE(sq,sq-9,EMPTY,EMPTY,MFLAGEP),list);
      if(!SQOFFBOARD(sq-11) && sq-11==pos->enpass)
        addenpassantmove(pos,MOVE(sq,sq-11,EMPTY,EMPTY,MFLAGEP),list);
    }
  }

  /* Loop for slide pieces */  
  
  pceindex = loopslideindex[side];
  pce = loopslidepce[pceindex++];
  
  while(pce != 0)
  {
    ASSERT(piecevalid(pce));
    //printf("slider pceindex:%d pce : %d\n",pceindex,pce);
    
    for(pceNum =0; pceNum < pos->pceNum[pce];pceNum++)
    {
      sq=pos->plist[pce][pceNum];
      ASSERT(sqonboard(sq));
      //printf("Piece : %c on %s\n",pcechar[pce],prsq(sq));
      
      for(i=0;i<numdir[pce];i++)
      {
        dir = pcedir[pce][i];
        t_sq = sq +dir;
        
        while(!SQOFFBOARD(t_sq))
        {
          if(pos->pieces[t_sq] != EMPTY)
          {
            if(piececol[pos->pieces[t_sq]]== side^1)
            {//printf("\t\tCapture on %s \n", prsq(t_sq));
             addcapturemove(pos,MOVE(sq,t_sq,pos->pieces[t_sq],EMPTY,0),list);
            }
            break;
          }
          t_sq += dir;
        }
      }
    }
    pce = loopslidepce[pceindex++];
  }
  /* Loop for slide pieces */
  
  /* Loop for non slide pieces */
  
  pceindex = loopnonslideindex[side];
  pce = loopnonslidepce[pceindex++];
  
  while(pce != 0)
  {
    ASSERT(piecevalid(pce));
    //printf("non slider pceindex:%d pce : %d\n",pceindex,pce);
    
    for(pceNum =0; pceNum <pos->pceNum[pce];pceNum++)
    {
      sq=pos->plist[pce][pceNum];
      ASSERT(sqonboard(sq));
      //printf("Piece : %c on %s\n",pcechar[pce],prsq(sq));
      for(i=0;i<numdir[pce];i++)
      {
        dir = pcedir[pce][i];
        t_sq = sq +dir;

        if(SQOFFBOARD(t_sq))
          continue;
        // BLACK ^ 1 == WHITE      WHITE ^ 1 == BLACK
        if(pos->pieces[t_sq] != EMPTY)
        {
          if(piececol[pos->pieces[t_sq]] == side ^ 1)
          {//printf("\t\tCapture on %s\n",prsq(t_sq));
            addcapturemove(pos,MOVE(sq,t_sq,pos->pieces[t_sq],EMPTY,0),list);}
          continue;
        }
        //printf("\t\tNormal on %s \n",prsq(t_sq));
      }
    }
    pce = loopnonslidepce[pceindex++];
  }

  /* Loop for non slide pieces */

}
