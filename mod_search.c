// search.c

#include "stdio.h"
#include "header.h"

#define INFINITE 30000
#define MATE 29000

static void checkUp() {
	// .. check if time up, or interrupt from GUI
}

static int isrepetition(const S_BOARD *pos) {

	int index = 0;

	for(index = pos->historyply - pos->fiftymove; index < pos->historyply-1; ++index) {	
		ASSERT(index >= 0 && index < MAXMOVES);		
		if(pos->poskey == pos->history[index].poskey) {
			return TRUE;
		}
	}	
	return FALSE;
}

static void clearforsearch(S_BOARD *pos, S_SEARCHINFO *info) {
	
	int index = 0;
	int index2 = 0;
	
	for(index = 0; index < 13; ++index) {
		for(index2 = 0; index2 < SQ_NUM; ++index2) {
			pos->searchhistory[index][index2] = 0;
		}
	}
	
	for(index = 0; index < 2; ++index) {
		for(index2 = 0; index2 < MAXDEPTH; ++index2) {
			pos->searchkillers[index][index2] = 0;
		}
	}	
	
	clearpvtable(pos->pvtable);	
	pos->ply = 0;
	
	info->starttime = gettime();
	info->stopped = 0;
	info->nodes = 0;
	info->fh = 0;
	info->fhf = 0;
}

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {
	return 0;
}

static int alphabeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNull) {

	ASSERT(checkboard(pos)); 
	
	if(depth == 0) {
		info->nodes++;
		return evalposition(pos);
	}
	
	info->nodes++;
	
	if(isrepetition(pos) || pos->fiftymove >= 100) {
		return 0;
	}
	
	if(pos->ply > MAXDEPTH - 1) {
		return evalposition(pos);
	}
	
	S_MOVELIST list[1];
    generateallmoves(pos,list);
      
    int MoveNum = 0;
	int Legal = 0;
	int OldAlpha = alpha;
	int BestMove = NOMOVE;
	int Score = -INFINITE;
	
	for(MoveNum = 0; MoveNum < list->count; ++MoveNum) {	
       
        if ( !makemove(pos,list->moves[MoveNum].move))  {
            continue;
        }
        
		Legal++;
		Score = -alphabeta( -beta, -alpha, depth-1, pos, info, TRUE);		
        takemove(pos);
		
		if(Score > alpha) {
			if(Score >= beta) {
				if(Legal==1) {
					info->fhf++;
				}
				info->fh++;				
				return beta;
			}
			alpha = Score;
			BestMove = list->moves[MoveNum].move;
		}		
    }
	
	if(Legal == 0) {
		if(sqattacked(pos->kingsq[pos->side],pos->side^1,pos)) {
			return -MATE + pos->ply;
		} else {
			return 0;
		}
	}
	
	if(alpha != OldAlpha) {
		storepvmove(pos, BestMove);
	}
	
	return alpha;
} 

void searchposition(S_BOARD *pos, S_SEARCHINFO *info) {
	
	int bestMove = NOMOVE;
	int bestScore = -INFINITE;
	int currentDepth = 0;
	int pvMoves = 0;
	int pvNum = 0;
	
	clearforsearch(pos,info);
	
	// iterative deepening
	for( currentDepth = 1; currentDepth <= info->depth; ++currentDepth ) {
							// alpha	 beta
		bestScore = alphabeta(-INFINITE, INFINITE, currentDepth, pos, info, TRUE);
		
		// out of time?
		
		pvMoves = getpvline(currentDepth, pos);
		bestMove = pos->pvarray[0];
		
		printf("Depth:%d score:%d move:%s nodes:%ld ",
			currentDepth,bestScore,prmove(bestMove),info->nodes);
			
		pvMoves = getpvline(currentDepth, pos);	
		printf("pv");		
		for(pvNum = 0; pvNum < pvMoves; ++pvNum) {
			printf(" %s",prmove(pos->pvarray[pvNum]));
		}
		printf("\n");
		printf("Ordering:%.2f\n",(info->fhf/info->fh));
	}
	
	
}




















