//data.c
#include "header.h"
char pcechar[] = ".PNBRQKpnbrqk";
char sidechar[] = "wb-";
char rankchar[] = "12345678";
char filechar[] = "abcdefgh";

int piecebig[13] = {FALSE, FALSE,TRUE,TRUE,TRUE,TRUE,TRUE,FALSE,TRUE,TRUE,TRUE,TRUE,TRUE};
int piecemaj[13] = {FALSE,FALSE,FALSE,FALSE,TRUE,TRUE,TRUE,FALSE,FALSE,FALSE,TRUE,TRUE,TRUE};
int piecemin[13] = {FALSE, FALSE,TRUE,TRUE,FALSE,FALSE,FALSE,FALSE,TRUE,TRUE,FALSE,FALSE,FALSE};
int pieceval[13] = {0,100,325,325,550,1000,50000,100,325,325,550,1000,50000};
int piececol[13] = {BOTH,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK};

int piecepawn[13] ={FALSE,TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE,FALSE,FALSE,FALSE,FALSE,FALSE};
int pieceknight[13] ={FALSE,FALSE,TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE,FALSE,FALSE,FALSE,FALSE};
int pieceking[13] ={FALSE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE,FALSE,FALSE,FALSE,FALSE,FALSE,TRUE};
int piecerookqueen[13] ={FALSE,FALSE,FALSE,FALSE,TRUE,TRUE,FALSE,FALSE,FALSE,FALSE,TRUE,TRUE,FALSE};
int piecebishopqueen[13] = {FALSE,FALSE,FALSE,TRUE,FALSE,TRUE,FALSE,FALSE,FALSE,TRUE,FALSE,TRUE,FALSE};

/*const int mirror64[64] = {

56  , 57  , 58  , 59  , 60  , 61  , 62  , 63  ,

48  , 49  , 50  , 51  , 52  , 53  , 54  , 55  ,

40  , 41  , 42  , 43  , 44  , 45  , 46  , 47  ,

32  , 33  , 34  , 35  , 36  , 37  , 38  , 39  ,

24  , 25  , 26  , 27  , 28  , 29  , 30  , 31  ,

16  , 17  , 18  , 19  , 20  , 21  , 22  , 23  ,

8   ,   9 , 10  , 11  , 12  , 13  , 14  , 15  ,

0   ,   1 ,   2 ,   3 ,   4 ,   5 ,   6 ,  7

};*/
