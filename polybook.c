#include "header.h"
#include "polykeys.h"

const int polykindofpiece[13] = { -1, 1, 3, 5, 7, 9, 11, 0, 2, 4, 6, 8, 10 };

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

u64 polykeyfromboard(S_BOARD *board) 
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















































