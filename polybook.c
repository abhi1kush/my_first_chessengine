#include "header.h"
#include "polykeys.h"

typedef struct {
  u64 key;
  unsigned short move;
  unsigned short weight;
  unsigned int learn;
}S_POLY_BOOK_ENTRY;

long numentries = 0;

S_POLY_BOOK_ENTRY *entries;

const int polykindofpiece[13] = { -1, 1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10 };

void initpolybook()
{ 
  engineoptions->usebook = FALSE;
  FILE *pFile = fopen("performance.bin","rb");
  if(pFile == NULL)
    printf("Book File Not Read\n");
  else 
  {
    fseek(pFile,0,SEEK_END);
    long position = ftell(pFile);

    if(position < sizeof(S_POLY_BOOK_ENTRY))
    {
      printf("No Entries Found\n");
      return;
    }
    numentries = position/sizeof(S_POLY_BOOK_ENTRY);
    printf("%ld Entries Found In File\n",numentries);

    entries = (S_POLY_BOOK_ENTRY *)malloc(numentries * sizeof(S_POLY_BOOK_ENTRY));
    rewind(pFile);

    size_t returnValue;
    returnValue = fread(entries,sizeof(S_POLY_BOOK_ENTRY),numentries,pFile); 
    printf("fread() %ld Entries Read in from file\n",returnValue);
    if(numentries > 0)
      engineoptions->usebook = TRUE;

  }
}

void cleanpolybook()
{
  free(entries);
}

int haspawnforcapture(const S_BOARD *board) 
{
	int sqwithpawn = 0;
	int targetpce = (board->side == WHITE) ? wp : bp;
	if(board->enpass != NO_SQ) 
  {
		if(board->side == WHITE) 
			sqwithpawn = board->enpass - 10;
    else 
			sqwithpawn = board->enpass + 10;
		
    if(board->pieces[sqwithpawn + 1] == targetpce) 
			return TRUE;
    else if(board->pieces[sqwithpawn - 1] == targetpce) 
			return TRUE;
	}
	return FALSE;
}


u64 polykeyfromboard(const S_BOARD *board) 
{
	int sq = 0, rank = 0, file = 0;
	u64 finalKey = 0;
	int piece = EMPTY;
	int polypiece = 0;
	int offset = 0;
	
	for(sq = 0; sq < SQ_NUM; ++sq) 
  {
		piece = board->pieces[sq];
		if(piece != NO_SQ && piece != EMPTY && piece != OFFBOARD) 
    {
			ASSERT(piece >= wp && piece <= bk);
			polypiece = polykindofpiece[piece];
			rank = ranksbrd[sq];
			file = filesbrd[sq];
			finalKey ^= random64poly[(64 * polypiece) + (8 * rank) + file];
		}
	}
	
	// castling
	offset = 768;
	
	if(board->castle & WO_O) finalKey ^= random64poly[offset + 0];
	if(board->castle & WO_O_O) finalKey ^= random64poly[offset + 1];
	if(board->castle & BO_O) finalKey ^= random64poly[offset + 2];
	if(board->castle & BO_O_O) finalKey ^= random64poly[offset + 3];
	
	// enpassant
	offset = 772;
	if(haspawnforcapture(board) == TRUE) {
		file = filesbrd[board->enpass];
		finalKey ^= random64poly[offset + file];
	}
	
	if(board->side == WHITE) {
		finalKey ^= random64poly[780];
	}
	return finalKey;
}

unsigned short endian_swap_u16(unsigned short x)
{
  x = (x>>8) | (x<<8);
  return x;
}

unsigned int endian_swap_u32(unsigned int x)
{
  x = (x>>24) | ( (x<<8) & 0x00FF0000) | ((x>>8) & 0x0000FF00) | (x<<24);
  return x;
}

u64 endian_swap_u64(u64 x)
{
  x = (x>>56) |
    ((x<<40) & 0x00FF000000000000) |
    ((x<<24) & 0x0000FF0000000000) |
    ((x<<8)  & 0x000000FF00000000) |
    ((x>>8)  & 0x00000000FF000000) |
    ((x>>24) & 0x0000000000FF0000) |
    ((x>>40) & 0x000000000000FF00) |
    (x<<56);
  return x;
}

int ConvertPolyMoveToInternalMove(int polyMove, S_BOARD *board)
{
  int ff = (polyMove >> 6) & 7;
  int fr = (polyMove >> 9) & 7;
  int tf = (polyMove >> 0) & 7;
  int tr = (polyMove >> 3) & 7;
  int pp = (polyMove >> 12) & 7;

  char moveString[6];
  if(pp == 0)
  {
    sprintf(moveString,"%c%c%c%c",
        filechar[ff],
        rankchar[fr],
        filechar[tf],
        rankchar[tr]);
  }
  else
  {
    char promchar = 'q';
    switch(pp)
    {
      case 1: promchar = 'n';break;
      case 2: promchar = 'b';break;
      case 3: promchar = 'r';break;
    }
    sprintf(moveString,"%c%c%c%c%c",filechar[ff],rankchar[fr],filechar[tf],rankchar[tr],promchar);
  }
  return parsemove(moveString,board);

}

void listbookmoves(u64 polykey,S_BOARD *board)
{
  int i = 0;
  S_POLY_BOOK_ENTRY * entry;
  unsigned short move;
  const int MAXBOOKMOVES = 32;
  int bookmoves[MAXBOOKMOVES];
  int tempMove;
  int count = 0;

  for(entry = entries; entry < entries + numentries; entry++)
  {
    if(polykey == endian_swap_u64(entry->key))
    {
      move = endian_swap_u16(entry->move);
      tempMove = ConvertPolyMoveToInternalMove(move,board);
      if(tempMove != NOMOVE)
      {
        bookmoves[count++] = tempMove;
        if(count > MAXBOOKMOVES)
          break;
      }
      /*printf("key:%llx Move:%c%c%c%c\n",
          endian_swap_u64(entry->key),
          filechar[(move >> 6) & 7],
          rankchar[(move >> 9) & 7],
          filechar[(move >> 0) & 7],
          rankchar[(move >> 3) & 7]);
      */
    }
  }
  printf("Listing Book Moves:\n");  
  for(i =0;i < count ; i++)
    printf("BookMove :%d : %s\n",i+1,prmove(bookmoves[i]));

}

void getbookmove(S_BOARD * board)
{
  u64 polykey = polykeyfromboard(board);
  printf("polykey :%llx\n",polykey);
  listbookmoves(polykey,board);

}














































