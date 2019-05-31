#include <stdio.h>

#define IF 1
#define THEN 2

extern FILE* yyin;
//extern int yylval;
//extern int yytext;
//extern int yyleng;

int yylex(){
  static int state = 0;
  static char stack[10] = {};
  static int i = 0;
  static char* yytext;
  static int yyleng = 0;
  
  while(!feof(yyin)){
    char c = fgetc(yyin);
    stack[i] = c;
    yytext = &stack[i];
    i++;
    yyleng++;

    switch(state){
      case 0:
        if(c == 'i') state = 1;
        else if(c == 't') state = 3;
        else return -1;
        break;
      case 1:
        if(c == 'f') state = 2;
        else return -1;
        break;
      case 2:
        fputc(c, yyin);
        return IF;
      case 3:
        if(c == 'h') state = 4;
        else return -1;
        break;
      case 4:
        if(c == 'h') state = 4;
        else return -1;
        break;
      case 5:
        if(c == 'h') state = 4;
        else return -1;
        break;
      case 6:
        fputc(c, yyin);
        return THEN;
    }
  }
}
