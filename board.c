//board.c

#include "stdio.h"
#include "header.h"

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
      case 'P': piece = bP; break;
      case 'R': piece = bR; break;
      case 'N': piece = bN; break;
      case 'B': piece = bB; break;
      case 'K': piece = bK; break;
      case 'Q': piece = bQ; break;

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
  }
}


void resetboard(S_BOARD *pos)
{
  int i=0;
  for(i=0;i<SQ_NUM;i++)
    pos-> pieces[i]=OFFBOARD;

  for(i=0;i<64;i++)
    pos->pieces[SQ120(i)]=EMPTY;

  for(i=0;i<3;i++)
  {
    pos->bigpce[i]=0;
    pos->majpce[i]=0;
    pos->minpce[i]=0;
    pos->pawns[i]=0ULL;
  }

  for(i=0;i<13;i++)
    pos->pceNo[i]=0;

  pos->kingsq[WHITE] = pos->king[BLACK] = NO_SQ;

  pos->side = BOTH;
  pos->enpass =NO_SQ;
  pos->fiftymove = 0;

  pos->ply=0;
  pos->historyply = 0;

  pos->castle = 0;
  pos->poskey = 0ULL;

}
