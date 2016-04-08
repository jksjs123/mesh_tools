%{

extern int line_num;
extern char *cval;
extern int ival;
extern double rval;
extern char err_msg[128];
char *my_strdup();

%}

%option outfile="lex.flex.c"
%option noyywrap

%p 10000
%a 10000 
%o 10000
%e 2000
%n 1000

%%

[ \t]+			;
^"#"[^\n]*$		;
"v"		{return(VERTEX);}
"f"			{return(FACE);}
"s"			{return(SMOOTH_GROUP);}
"off"		{return(OFF);}
([-]*[0-9]+\.) |
([-]*[0-9]*\.[0-9]+) |
([-]*[0-9]+[eE][+-]?[0-9]+) |
([-]*[0-9]+\.[0-9]*[eE][+-]?[0-9]+) |
([-]*[0-9]*\.[0-9]+[eE][+-]?[0-9]+) {sscanf((const char *)yytext,"%lf",&rval); return(REAL);}
([-]*[0-9]+)            {sscanf((const char *)yytext,"%d",&ival); return(INTEGER);}
.			;
\n			{line_num++;}