//xboard.c

#include "stdio.h"
#include "header.h"
#include <string.h>

//position startpos moves e2e4 e7e5 ... .. .. ... ...
//go wtime ......
//usermove e2e4
//move e7e5

int threefoldrep(const S_BOARD *pos)
{
  int i=0, r =0;
  for(i=0;i<pos->historyply;i++)
  {
    if(pos->history[i].poskey == pos->poskey)
    {
      r++;
    }
  }
  return r  ;
}

int drawmaterial(const S_BOARD *pos)
{
  if(pos->pceNum[wp] || pos->pceNum[bp]) return FALSE;
  if(pos->pceNum[wq] || pos->pceNum[bq] || pos->pceNum[wr] || pos->pceNum[br]) return FALSE;
  if(pos->pceNum[wb] > 1 || pos->pceNum[bb] >1) return FALSE;
  if(pos->pceNum[wn] > 1 || pos->pceNum[bn] >1) return FALSE;
  if(pos->pceNum[wn] && pos->pceNum[wb]) return FALSE;
  if(pos->pceNum[bn] && pos->pceNum[bb]) return FALSE;
  return TRUE;
}

int checkresult(S_BOARD *pos)
{
  ASSERT(checkboard(pos));
  
  if(pos->fiftymove >100)
  {
    printf("1/2-1/2 {fifty move rule (claimed by abhi_engine)\n}");
    return TRUE;
  }

  if(threefoldrep(pos)>=2)
  {
    printf("1/2-1/2 {3-fold repetition (claimed byabhi_engine)\n}");
    return TRUE;
  }
  
  if(drawmaterial(pos)==TRUE)
  {
    printf("1/2-1/2 {insufficient material (claimed by abhi_engine)}\n");
    return TRUE;
  }

  S_MOVELIST list[1];
  generateallmoves(pos,list);

  int movenum = 0;
  int found =0;
  for(movenum = 0; movenum < list->count; movenum++)
  {
    if(!makemove(pos,list->moves[movenum].move))
      continue;
    found++;
    takemove(pos);
    break;
  }
  if(found != 0)
    return FALSE;

  int incheck = sqattacked(pos->kingsq[pos->side],pos->side^1,pos);
  if(incheck == TRUE)
  {
    if(pos->side == WHITE)
    {
      printf("0-1 {black mates (claimed by abhi_engine)}\n");
      return TRUE;
    }
    else
    {
      printf("1-0 {white mates (claimed by abhi_engine)}\n");
      return TRUE;
    }
  }
  else
  {
    printf("\n1/2-1/2 {stalemate (claimed by abhi_engine)}\n");
    return TRUE;
  }
  return FALSE;
}


void printoptions() 
{
    printf("feature ping=1 setboard=1 colors=0 usermove=1\n");      
    printf("feature done=1\n");
}

void xboard_loop(S_BOARD *pos,S_SEARCHINFO *info) 
{
  info->GAME_MODE = XBOARDMODE;
  info->POST_THINKING = TRUE;
 
  setbuf(stdin, NULL);
  setbuf(stdout, NULL);
  
  printoptions(); // HACK

  int depth = -1, movestogo[2] = {30,30}, movetime = -1;
  int time = -1, inc = 0;                             
  int engineSide = BOTH;                    
  int timeLeft;   
  int sec;
  int mps;
  int MB=16;
  int move = NOMOVE;  
  //int i, score;
  char inBuf[80], command[80];  
  
  engineSide = BLACK; 
  parse_fen(START_FEN, pos);

  while(TRUE) 
  { 
    fflush(stdout);
    if(pos->side == engineSide && checkresult(pos) == FALSE) 
    {  
      info->starttime = gettime();
      info->depth = depth;
      
      if(time != -1) 
      {
        info->timeset = TRUE;
        time /= movestogo[pos->side];
        time -= 50;   
        info->stoptime = info->starttime + time + inc;
      } 
      
      if(depth == -1 || depth > MAXDEPTH) 
        info->depth = MAXDEPTH;
      
      printf("time:%5d start:%5ld stop:%5ld depth:%5d timeset:%5d movestogo:%5d mps:%5d\n",time,info->starttime,info->stoptime,info->depth,info->timeset, movestogo[pos->side], mps);
      searchposition(pos, info);
      
      if(mps != 0) 
      {
        movestogo[pos->side^1]--;
        if(movestogo[pos->side^1] < 1) 
          movestogo[pos->side^1] = mps;
      }
    } 
    fflush(stdout);

    memset(&inBuf[0], 0, sizeof(inBuf));

    fflush(stdout);
    if (!fgets(inBuf, 80, stdin))
      continue;

    sscanf(inBuf, "%s", command);
    printf("command seen:%s\n",inBuf);

    if(!strcmp(command, "quit")) 
    { 
      info->quit = TRUE;
      break; 
    }
    if(!strcmp(command, "force")) 
    { 
      engineSide = BOTH; 
      continue; 
    } 

    if(!strcmp(command, "protover"))
    {
      printoptions();
      continue;
    }
    if(!strcmp(command, "sd")) 
    {
      sscanf(inBuf, "sd %d", &depth); 
      printf("DEBUG depth:%d\n",depth);
      continue; 
    }
    if(!strcmp(command, "st")) 
    {
      sscanf(inBuf, "st %d", &movetime); 
      printf("DEBUG movetime:%d\n",movetime);
      continue; 
    }      
    if(!strcmp(command, "time")) 
    {
      sscanf(inBuf, "time %d", &time); 
      time *= 10;
      printf("DEBUG time:%d\n",time);
      continue; 
    }

    if(!strcmp(command,"memory"))
    {
      sscanf(inBuf,"memory %d",&MB);
      if(MB < 4) MB = 4;
      if(MB > MAX_HASH) MB = MAX_HASH;
      printf("Set Hash to %d MB\n",MB);
      inithashtable(pos->hashtable,MB);
      continue;
    }

    if(!strcmp(command, "level")) {
      sec = 0;
      movetime = -1;
      if(sscanf(inBuf, "level %d %d %d", &mps, &timeLeft, &inc) != 3) 
      {
        sscanf(inBuf, "level %d %d:%d %d", &mps, &timeLeft, &sec, &inc);
        printf("DEBUG level with :\n");
      } 
      else 
      {
          printf("DEBUG level without :\n");
      }     

      timeLeft *= 60000;
      timeLeft += sec * 1000;
      movestogo[0] = movestogo[1] = 30;
      if(mps != 0) {
        movestogo[0] = movestogo[1] = mps;
      }
      time = -1;
      printf("DEBUG level timeLeft:%d movesToGo:%d inc:%d mps%d\n",timeLeft,movestogo[0],inc,mps);
      continue; 
    }     

    if(!strcmp(command, "ping")) { 
      printf("pong%s\n", inBuf+4); 
      continue; 
    }

    if(!strcmp(command, "new")) 
    { 
      clearhashtable(pos->hashtable);
      engineSide = BLACK; 
      parse_fen(START_FEN, pos);
      depth = -1; 
      time = -1;
      continue; 
    }

    if(!strcmp(command, "setboard")){
      engineSide = BOTH;  
      parse_fen(inBuf+9, pos); 
      continue; 
    }       

    if(!strcmp(command, "go")) { 
      engineSide = pos->side;  
      continue; 
    }   
   /* 
    if(!strcmp(command,"bookmove"))
    {
      printboard(pos);
      printf("BookMove %s \n",getbookmove(pos);
      continue;
    }
*/
    if(!strcmp(command, "usermove"))
    {
      movestogo[pos->side]--;
      move = parsemove(inBuf+9, pos); 
      if(move == NOMOVE) 
        continue;
      makemove(pos, move);
      pos->ply=0;
    }

  } 
}



void console_loop(S_BOARD *pos, S_SEARCHINFO *info) {

	printf("Welcome to abhi_engine In Console Mode!\n");
	printf("Type help for commands\n\n");

	info->GAME_MODE = CONSOLEMODE;
	info->POST_THINKING = TRUE;
	setbuf(stdin, NULL);
  setbuf(stdout, NULL);
	
	int depth = MAXDEPTH, movetime = 3000;            
	int engineSide = BOTH;    
	int move = NOMOVE;		
	char inBuf[80], command[80];	
	
	engineSide = BLACK; 
	parse_fen(START_FEN, pos);	
	
	while(TRUE) { 

		fflush(stdout);

		if(pos->side == engineSide && checkresult(pos) == FALSE) 
    {  
			info->starttime = gettime();
			info->depth = depth;
			
			if(movetime != 0) 
      {
				info->timeset = TRUE;
				info->stoptime = info->starttime + movetime;
			} 	
			searchposition(pos, info);
		}	
		
		printf("\nabhi_engine > ");

		fflush(stdout); 
	
		memset(&inBuf[0], 0, sizeof(inBuf));
		fflush(stdout);

		if (!fgets(inBuf, 80, stdin))
		continue;
		sscanf(inBuf, "%s", command);
		
		if(!strcmp(command, "help")) { 
			printf("Commands:\n");
			printf("quit - quit game\n");
			printf("force - computer will not think\n");
			printf("print - show board\n");
			printf("post - show thinking\n");
			printf("nopost - do not show thinking\n");
			printf("new - start new game\n");
			printf("setboard - set position using fen string\n");
			printf("go - set computer thinking\n");
			printf("depth x - set depth to x\n");
			printf("time x - set thinking time to x seconds (depth still applies if set)\n");
			printf("view - show current depth and movetime settings\n");
			printf("** note ** - to reset time and depth, set to 0\n");
			printf("enter moves using b7b8q notation\n\n\n");
			continue;
		}
    
    if(!strcmp(command,"mirror"))
    {
      printboard(pos);
      printf("Eval %d\n",evalposition(pos));
      mirrorboard(pos);
      printboard(pos);
      printf("Eval : %d\n",evalposition(pos));
      mirrorboard(pos);
      continue;
    }

    
		if(!strcmp(command, "quit")) { 
			info->quit = TRUE;
			break; 
		}
		
		if(!strcmp(command, "post")) { 
			info->POST_THINKING = TRUE;
			continue; 
		}
		
		if(!strcmp(command, "print")) { 
			printboard(pos);
			continue; 
		}
		
		if(!strcmp(command, "nopost")) { 
			info->POST_THINKING = FALSE;
			continue; 
		}
		
		if(!strcmp(command, "force")) { 
			engineSide = BOTH; 
			continue; 
		} 
		
		if(!strcmp(command, "view")) 
    {
			if(depth == MAXDEPTH) 
        printf("depth not set ");
			else 
        printf("depth %d",depth);
			
			if(movetime != 0) 
        printf(" movetime %ds\n",movetime/1000);
			else 
        printf(" movetime not set\n");
			continue; 
		}
		
		if(!strcmp(command, "depth")) 
    {
			sscanf(inBuf, "depth %d", &depth); 
		  if(depth==0) 
        depth = MAXDEPTH;
			continue; 
		}
		
		if(!strcmp(command, "time")) {
			sscanf(inBuf, "time %d", &movetime); 
			movetime *= 1000;
			continue; 
		} 
		
		if(!strcmp(command, "new")) { 
			clearhashtable(pos->hashtable);
      engineSide = BLACK; 
			parse_fen(START_FEN, pos);
			continue; 
		}
    
    if(!strcmp(command, "setboard")){
      engineSide = BOTH;  
      parse_fen(inBuf+9, pos); 
      continue; 
    }       
			
		if(!strcmp(command, "go")) { 
			engineSide = pos->side;  
			continue; 
		}	
    
		move = parsemove(inBuf, pos);	
		if(move == NOMOVE) 
    {
			printf("Command unknown:%s\n",inBuf);
			continue;
		}
		makemove(pos, move);
    pos->ply=0;
  }	
}

