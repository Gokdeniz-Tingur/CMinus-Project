/***********************************************************/
/*    FILE: scan.c                                         */
/* PURPOSE: The scanner implementation for the C- compiler */
/* AUTHORS: YOUR NAMES HERE                                */
/*  COURSE: COMSC 440.01 24/SP                             */
/*    DATE: Feb 26, 2024                                */
/***********************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
/* States in scanner DFA */
//(1)Add you code to define all states that C- DFA needs as an enum type called StateType in C 
typedef enum {START, INLESS, INEQ, INGREATER, INNEQ, ENTERCOMMENT, INCOMMENT, EXITCOMMENT, INNUM, INID, DONE} StateType; 

/* The lexeme of the ID or reserved word */
char tokenString[MAXTOKENLEN + 1];

/* BUFLEN = length of the input buffer for source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* Holds the current line */
static int linepos = 0;		 /* Current position in lineBuf */
static int bufsize = 0;		 /* Current size of the buffer string */
static int EOF_flag = FALSE; /* Corrects ungetNextChar behavior on EOF */

/* getNextChar() fetches the next non-blank from lineBuf, reading in a 
   new line if lineBuf is exhausted. */
static int getNextChar(void)
{
	if (!(linepos < bufsize))
	{
		lineno++;
		if (fgets(lineBuf, BUFLEN - 1, source))
		{
			if (EchoSource)
				fprintf(listing, "%4d: %s", lineno, lineBuf);
			bufsize = strlen(lineBuf);
			linepos = 0;
			return lineBuf[linepos++];
		}
		else
		{
			EOF_flag = TRUE;
			return EOF;
		}
	}
	else
		return lineBuf[linepos++];
}

/* ungetNextChar() backtracks one character in lineBuf */
static void ungetNextChar(void)
{
	if (!EOF_flag)
		linepos--;
}

/* A lookup table of reserved words */
//(2) Define a struct called reservedWords, with the appropriate size, in C that represents the C- reserved words
static struct
{ char* str;
  TokenType tok;
} reservedWords[MAXRESERVED]
= {{"if",IF},{"else",ELSE},
  {"int",INT},{"return",RETURN},{"void",VOID},
  {"while",WHILE}};

/* Looks up an ID to see if it is a reserved word */
//(3) Define the function ReservedLookupB(), which search regular IDs againist the C- reserved words
//    Use binary search instead of linear search
int compareWords(const void *a, const void *b) {
    return strcmp(((typeof(reservedWords[0])*)a)->str, ((typeof(reservedWords[0])*)b)->str);
}

static TokenType ReservedLookupB(int low, int high, char *s) {
		static int sorted = 0;

		if (!sorted) {
			qsort(reservedWords, MAXRESERVED, sizeof(reservedWords[0]), compareWords);
			sorted = 1; 
		}

		if (low > high)
			return ID;
	
		int mid = (low + high) / 2;
		int cmp = strcmp(s, reservedWords[mid].str);
	
		if (cmp == 0)
			return reservedWords[mid].tok;
		else if (cmp < 0)
			return ReservedLookupB(low, mid - 1, s);
		else
			return ReservedLookupB(mid + 1, high, s);
	}


/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the 
 * next token in source file
 */
TokenType getToken(void)
{ /* index for storing into tokenString */
	int tokenStringIndex = 0;
	/* holds current token to be returned */
	TokenType currentToken;
	/* current state - always begins at START */
	StateType state = START;
	/* flag to indicate save to tokenString */
	int save;
	while (state != DONE)
	{
		int c = getNextChar();
		save = TRUE;

		switch (state)
		{
		case START:
			if (isdigit(c)) {
				save = TRUE; // save digits for the number
				state = INNUM;
			} else if (isalpha(c)) {
				save = TRUE; // save letters for the identifier
				state = INID;
			} else if (c == '<') {
				save = FALSE;
				state = INLESS;
			} else if (c == '=') {
				save = FALSE;
				state = INEQ;
			} else if (c == '>') {
				save = FALSE;
				state = INGREATER;
			} else if (c == '!') {
				save = FALSE;
				state = INNEQ;
			} else if (c == '/') {
				save = FALSE;
				state = ENTERCOMMENT;
			}
         
         //(5) create the else if branch to handle the comments: /* */

			else if ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r')) //white spaces 
				save = FALSE;
			else   //single character operators
			{
				state = DONE;
				switch (c)
				{
				case EOF:
					save = FALSE;
					currentToken = ENDFILE;
					break;
				case '+':
					currentToken = PLUS;
               break;
               //(7) create cases to handle operators: -, *, ;, ,. (, ), [, ], {, }
				case '-':
					currentToken = MINUS;
					break;
				case '*':
					currentToken = TIMES;
					break;

				case ';':	
					currentToken = SEMI;
					break;
				case ',':	
					currentToken = COMMA;
					break;
				case '(':	
					currentToken = LPAREN;
					break;
				case ')':
					currentToken = RPAREN;
					break;
				case '[':
					currentToken = LBRACKET;
					break;
				case ']':
					currentToken = RBRACKET;
					break;
				case '{':
					currentToken = LCURLY;
					break;
				case '}':
					currentToken = RCURLY;
					break;
				default:
					currentToken = ERROR;
					break;
				}
			}
			break;

		case INLESS:
			state = DONE;
			if (c == '=') {			// <=
				currentToken = LE;
			} else {				// <
				ungetNextChar();
				currentToken = LT;
			}
			break;

		//(8) handle the case of INEQ
      case INEQ:
			state = DONE;
			if(c == '=') {			// ==
				currentToken = EQ;
			} else {				// =
				ungetNextChar();
				currentToken = ASSIGN;
			}
			break;

		//(9) handle the case of INGREATER
      case INGREATER:
			state = DONE;
			if (c == '=') {			// >=
				currentToken = GE;
			} else {				// >
				ungetNextChar();
				currentToken = GT;
			}
			break;

		//(10) handle the case of INNEQ
      case INNEQ:
			state = DONE;
			if (c == '=') {
				currentToken = NE; // "!="
			} else {
				ungetNextChar();
				currentToken = ERROR; //just "!" is invalid
			}
			break;
		

		//(11) handle the case of INNUM
      case INNUM:
			if (!isdigit(c))
			{
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = NUM;
			}
			break;

		//(12) handle the case of INID 
      case INID:
			if (!isalpha(c))
			{
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = ID;
			}
         	break;

		case ENTERCOMMENT:
			if (c == '*') {
				state = INCOMMENT;
			} else {
				state = DONE;
				currentToken = OVER;
			}
			break;
		
		case INCOMMENT:
			if (c == '*') {
				state = EXITCOMMENT;
			}
			break;
	
		case EXITCOMMENT:
			if (c == '/') {
				state = START;
			} else {
				state = INCOMMENT;
			}
			break;

		case DONE:
			break;
		default:
			fprintf(listing, "Scanner Bug: state= %d\n", state);
			state = DONE;
			currentToken = ERROR;
			break;
		}
		if ((save) && (tokenStringIndex <= MAXTOKENLEN))
			tokenString[tokenStringIndex++] = (char)c;
		if (state == DONE)
		{
			tokenString[tokenStringIndex] = '\0';
			if (currentToken == ID)
				currentToken = ReservedLookupB(0, MAXRESERVED - 1, tokenString);
		}
	}
	if (TraceScan)
	{
		fprintf(listing, "\t%d: ", lineno);
		printToken(currentToken, tokenString);
	}
	return currentToken;
} /* end getToken */
