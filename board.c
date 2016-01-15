//board.c

#include "stdio.h"
#include "header.h"

int checkboard(const S_BOARD *pos)
{
  int t_pceNum[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};
  int t_bigpce[2] = {0,0};
  int t_majpce[2] = {0,0};
  int t_minpce[2] = {0,0};
  int t_material[2] = {0,0};

  int sq64 , t_piece, t_pce_num, sq120, color, pcount;
  
  u64 t_pawns[3]= {0ULL,0ULL,0ULL};

  t_pawns[WHITE] = pos->pawns[WHITE];
  t_pawns[BLACK] = pos->pawns[BLACK];
  t_pawns[BOTH] = pos->pawns[BOTH];

  //CHECK PIECE LISTS 
  for(t_piece = wp;t_piece <= bk;++t_piece)
    for(t_pce_num=0;t_pce_num< pos->pceNum[t_piece];t_pce_num++)
    {
      sq120=pos->plist[t_piece][t_pce_num];
      ASSERT(pos->pieces[sq120]==t_piece);
    }
  //check piece count and other counters 
  for(sq64 =0;sq64<64;sq64++)
  {
    sq120=SQ120(sq64);
    t_piece = pos->pieces[sq120];
    t_pceNum[t_piece]++;
    color = piececol[t_piece];
    if(piecebig[t_piece]==TRUE) t_bigpce[color]++;
    if(piecemin[t_piece]==TRUE) t_minpce[color]++;
    if(piecemaj[t_piece]==TRUE) t_majpce[color]++;

    t_material[color] += pieceval[t_piece];

  }

  for(t_piece = wp;t_piece <= bk; ++t_piece)
  {
    if(!(t_pceNum[t_piece]==pos->pceNum[t_piece]))
      printf("\nt_piece %d , t_pceNum[t_piece]: %d  pos->pceNum[t_piece]: %d\n",t_piece,t_pceNum[t_piece],pos->pceNum[t_piece]);
    ASSERT(t_pceNum[t_piece]==pos->pceNum[t_piece]);
  }

  //check bitboard count
  pcount = CNT(t_pawns[WHITE]);
  ASSERT(pcount == pos->pceNum[wp]);
  pcount = CNT(t_pawns[BLACK]);
  ASSERT(pcount == pos->pceNum[bp]);
  pcount = CNT(t_pawns[BOTH]);
  ASSERT(pcount == (pos->pceNum[bp] + pos->pceNum[wp]));
  
  // check bitboards squres
  while(t_pawns[WHITE])
  {
    sq64 = POP(&t_pawns[WHITE]);
    ASSERT(pos->pieces[SQ120(sq64)] == wp);
  }
  
  while(t_pawns[BLACK])
  {
    sq64 = POP(&t_pawns[BLACK]);
    ASSERT(pos->pieces[SQ120(sq64)] == bp);
  }
  while(t_pawns[BOTH])
  {
    sq64 = POP(&t_pawns[BOTH]);
    ASSERT( (pos->pieces[SQ120(sq64)] == bp) || (pos->pieces[SQ120(sq64)] == wp));
  }

  ASSERT(t_material[WHITE]==pos->material[WHITE] && t_material[BLACK]==pos->material[BLACK]);
  ASSERT(t_minpce[WHITE]==pos->minpce[WHITE] && t_minpce[BLACK] == pos->minpce[BLACK]);
  ASSERT(t_majpce[WHITE]==pos->majpce[WHITE] && t_majpce[BLACK] == pos->majpce[BLACK]);
  ASSERT(t_bigpce[WHITE]==pos->bigpce[WHITE] && t_bigpce[BLACK] == pos->bigpce[BLACK]);

  ASSERT(pos->side == WHITE || pos->side == BLACK);
  ASSERT(generateposkey(pos)==pos->poskey);

  ASSERT(pos->enpass==NO_SQ || (ranksbrd[pos->enpass]==RANK_6 && pos->side == WHITE)
      || (ranksbrd[pos->enpass]==RANK_3 && pos->side ==BLACK));
  
  ASSERT(pos->pieces[pos->kingsq[WHITE]]==wk);
  ASSERT(pos->pieces[pos->kingsq[BLACK]]==bk);

  return TRUE;
}

int parse_fen(char *fen, S_BOARD *pos)
{
  ASSERT(fen!=NULL);
  ASSERT(pos!=NULL);

  int rank = RANK_8;
  int file =FILE_A;
  int piece = 0;
  int count =0;
  int i=0;
  int sq64 =0;
  int sq120 =0;

  resetboard(pos);

  while((rank>=RANK_1) && *fen)
  { 
    count =1;
    switch(*fen)
    {
      case 'p': piece = bp; break;
      case 'r': piece = br; break;
      case 'n': piece = bn; break;
      case 'b': piece = bb; break;
      case 'k': piece = bk; break;
      case 'q': piece = bq; break;
      case 'P': piece = wp; break;
      case 'R': piece = wr; break;
      case 'N': piece = wn; break;
      case 'B': piece = wb; break;
      case 'K': piece = wk; break;
      case 'Q': piece = wq; break;

      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
        piece =EMPTY;
        count = *fen - '0';
        break;
      
      case '/':
      case ' ':
        rank--;
        file = FILE_A;
        fen++;
        continue;
      
      default:
        printf("FEN error \n");
        return -1;
    }
    //file A B C D E RANK 4
    // 4RKQP 
    for(i=0;i<count;i++)
    {
      sq64=rank * 8 + file;
      sq120 = SQ120(sq64);
      if(piece != EMPTY)
        pos->pieces[sq120] = piece;
      file++;
    }
    fen++;
  }
  ASSERT(*fen == 'w' || *fen == 'b');
  pos->side = (*fen == 'w') ? WHITE : BLACK;
  fen +=2;

  for(i=0;i<4;i++)
  {
    if(*fen == ' ')
      break;
    switch(*fen)
    {
      case 'K': pos->castle |= WO_O; break;
      case 'Q': pos->castle |= WO_O_O; break;
      case 'k': pos->castle |= BO_O; break;
      case 'q': pos->castle |= BO_O_O; break;
      defalut : break;
    }
    fen++;
  }
  fen++;

  ASSERT(pos->castle>=0 && pos->castle <=15);

  if(*fen != '-')
  {
    file = fen[0] - 'a';
    rank = fen[1] -'1';

    ASSERT(file>= FILE_A && file <= FILE_H);
    ASSERT(rank>=RANK_1 && rank <= RANK_8);

    pos->enpass = FR2SQ(file,rank);
  }

  pos->poskey = generateposkey(pos);
  
  return 0;
}



void resetboard(S_BOARD *pos)
{
  int i=0;
  for(i=0;i<SQ_NUM;i++)
    pos->pieces[i]=OFFBOARD;

  for(i=0;i<64;i++)
    pos->pieces[SQ120(i)]=EMPTY;

  for(i=0;i<2;i++)
  {
    pos->bigpce[i]=0;
    pos->majpce[i]=0;
    pos->minpce[i]=0;
    pos->material[i]=0;
  }
  for(i=0;i<3;i++)
    pos->pawns[i]=0ULL;

  for(i=0;i<13;i++)
    pos->pceNum[i]=0;

  pos->kingsq[WHITE] = pos->kingsq[BLACK] = NO_SQ;

  pos->side = BOTH;
  pos->enpass =NO_SQ;
  pos->fiftymove = 0;

  pos->ply=0;
  pos->historyply = 0;

  pos->castle = 0;
  pos->poskey = 0ULL;

}

void printboard(const S_BOARD *pos)
{
  int sq,file,rank,piece;
  printf("\n Game Board : \n\n");
  for(rank=RANK_8;rank>=RANK_1;rank--)
  {
    printf("%d ",rank+1);
    for(file=FILE_A;file<=FILE_H;file++)
    {
      sq=FR2SQ(file,rank);
      piece = pos->pieces[sq];
      printf("%3c",pcechar[piece]);
    }
    printf("\n");
  }

  printf("\n  ");
  for(file = FILE_A; file <=FILE_H;file++)
  {
    printf("%3c",'a'+file);
  }
  printf("\n");
  printf("side: %c\n",sidechar[pos->side]);
  printf("enpass : %d \n",pos->enpass);
  printf("castle :%c%c%c%c\n",
      pos->castle & WO_O ? 'K':'-',
      pos->castle & WO_O_O ? 'Q':'-',
      pos->castle & BO_O ? 'k':'-',
      pos->castle & BO_O_O ? 'q':'-'
      );
  printf("poskey %llX\n",pos->poskey);
      
}

void updatelistmaterial(S_BOARD *pos)
{
  int piece,sq,i,color;
  for(i=0;i<SQ_NUM;i++)
  {
    sq=i;
    piece = pos->pieces[i];
    if(piece != OFFBOARD && piece != EMPTY)
    {
      color=piececol[piece];
      if(piecebig[piece]==TRUE)
        pos->bigpce[color]++;
      if(piecemin[piece]==TRUE)
        pos->minpce[color]++;
      if(piecemaj[piece]==TRUE)
        pos->majpce[color]++;
      pos->material[color] += pieceval[piece];

      //piece list
      //// plist[wp][pcenum]
      pos->plist[piece][pos->pceNum[piece]]=sq;
      pos->pceNum[piece]++;

      if(piece==wk) 
        pos->kingsq[WHITE] = sq;
      if(piece==bk) 
        pos->kingsq[BLACK] = sq;
      
      if(piece==wp)
      {
        setbit(pos->pawns[WHITE],SQ64(sq));
        setbit(pos->pawns[BOTH],SQ64(sq));
      }
      else if(piece==bp)
      {
        setbit(pos->pawns[BLACK],SQ64(sq));
        setbit(pos->pawns[BOTH],SQ64(sq));
      }
       
    }
  }
}
