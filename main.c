#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

typedef enum{
	TK_PUNCT,
	TK_NUM,
	TK_EOF
}TokenKind;

typedef struct Token Token;
struct Token{
	TokenKind Kind;
	Token *Next;
	int Val;
	char *Loc;
	int Len;
};

static char *CurrentInput;

static void error(char *Fmt, ...) {
	va_list VA;
	va_start(VA,Fmt);
	vfprintf(stderr,Fmt,VA);
	fprintf(stderr,"\n");
	va_end(VA);
	exit(1);
}


static void verrorAt(char *Loc, char *Fmt,va_list VA)
{
	fprintf(stderr,"%s\n",CurrentInput);
	int Pos = Loc - CurrentInput;
	fprintf(stderr,"%*s",Pos," ");
	fprintf(stderr,"^ ");
	vfprintf(stderr,Fmt,VA);
	fprintf(stderr,"\n");
	va_end(VA);
	exit(1);
}

static void errorAt(char *Loc, char *Fmt, ...)
{
	va_list VA;
	va_start(VA,Fmt);
	verrorAt(Loc,Fmt,VA);
}

static void errorTok(Token* Tok,char *Fmt, ...) {
	va_list VA;
	va_start(VA,Fmt);
	verrorAt(Tok->Loc,Fmt,VA);
}


static bool equal(Token* Tok,char *Str) {
	return memcmp(Tok->Loc,Str,Tok->Len) == 0 && Str[Tok->Len] == '\0';
}

static Token *skip(Token *Tok, char *Str) {
	if(!equal(Tok,Str))
		errorTok(Tok,"expect '%s'",Str);
	return Tok->Next;
}

static int getNumber(Token *Tok)
{
	if(Tok->Kind !=TK_NUM){
		errorTok(Tok,"expect a number");
	}
	return Tok->Val;
}

static Token *newToken(TokenKind Kind, char *Start,char *End) {
	Token* Tok = calloc(1,sizeof(Token));
	Tok->Kind = Kind;
	Tok->Loc = Start;
	Tok->Len = End - Start;
	return Tok;
}

static Token *tokenize() {
	char *p = CurrentInput;
	Token Head = {};
	Token *Cur = &Head;
	while(*p) {
		if(isspace(*p)) {
			++p;
			continue;
		}
		
		if(isdigit(*p)) {
			Cur->Next = newToken(TK_NUM,p,p);
			Cur = Cur->Next;
			const char *OldPtr = p;
			Cur->Val = strtoul(p,&p,10);
			Cur->Len = p-OldPtr;
			continue;
		}
		
		if(*p == '+' || *p == '-') {
			Cur->Next = newToken(TK_PUNCT,p,p+1);
			Cur = Cur->Next;
			++p;
			continue;
		}
		errorAt(p,"invalid token");	
	}
	Cur->Next = newToken(TK_EOF,p,p);
	return Head.Next;
}

// rvcc的程序入口函数
int main(int Argc, char **Argv) {
  
    if(Argc !=2 ) {
    	error("%s: invalid number of arguments",Argv[0]);
    	return 1;
    }
    CurrentInput = Argv[1];
    
    Token *Tok = tokenize();
    
    printf("  .globl main\n");
    printf("main:\n");
    printf("li a0, %d\n",getNumber(Tok));
    Tok = Tok->Next;
    while(Tok->Kind != TK_EOF){
    	if(equal(Tok,"+")) {
    		Tok = Tok->Next;
    		printf("  addi a0, a0, %d\n", getNumber(Tok));
    		Tok = Tok->Next;
    		continue;
    	}
    	Tok = skip(Tok,"-");
    	printf("  addi a0, a0, -%d\n", getNumber(Tok));
    	Tok = Tok->Next;
    }
    
    printf("  ret\n");
  return 0;
}
