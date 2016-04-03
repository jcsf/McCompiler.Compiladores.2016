%X COMMENT
%{
	int line = 1;
	int col = 1;
	int start_line = 1;
	int start_col = 1;
	int flag = 0;
%}
letter				[a-zA-Z_]
number 				[0-9]
seq_escape			\\(n|t|\\|\'|\"|[0-7]{1,3})
reserved 			"auto"|"break"|"case"|"const"|"continue"|"default"|"do"|"double"|"enum"|"extern"|"float"|"goto"|"long"|"register"|"short"|"signed"|"sizeof"|"static"|"struct"|"switch"|"typedef"|"union"|"unsigned"|"volatile"|"while"|"++"|"--"
%%
"/*"								{BEGIN COMMENT; start_line = line; start_col = col; col+=yyleng;}
<COMMENT>.							{col+=yyleng;}
<COMMENT>\n							{line++; col=1;}
<COMMENT><<EOF>>					{printf("Line %d, col %d: unterminated comment\n", start_line, start_col); yyterminate();}
<COMMENT>"*/"						{BEGIN 0; col+=yyleng;}
"&"									{if(flag) printf("AMP\n"); col+=yyleng;}
"&&"								{if(flag) printf("AND\n"); col+=yyleng;}
"="									{if(flag) printf("ASSIGN\n"); col+=yyleng;}
"*"									{if(flag) printf("AST\n"); col+=yyleng;}
"char"								{if(flag) printf("CHAR\n"); col+=yyleng;}
","									{if(flag) printf("COMMA\n"); col+=yyleng;}
"/"									{if(flag) printf("DIV\n"); col+=yyleng;}
"else"								{if(flag) printf("ELSE\n"); col+=yyleng;}
"=="								{if(flag) printf("EQ\n"); col+=yyleng;}
"for"								{if(flag) printf("FOR\n"); col+=yyleng;}
">="								{if(flag) printf("GE\n"); col+=yyleng;}
">"									{if(flag) printf("GT\n"); col+=yyleng;}
"if"								{if(flag) printf("IF\n"); col+=yyleng;}
"int"								{if(flag) printf("INT\n"); col+=yyleng;}
"{"									{if(flag) printf("LBRACE\n"); col+=yyleng;}
"<="								{if(flag) printf("LE\n"); col+=yyleng;}
"("									{if(flag) printf("LPAR\n"); col+=yyleng;}
"["									{if(flag) printf("LSQ\n"); col+=yyleng;}
"<"									{if(flag) printf("LT\n"); col+=yyleng;}
"-"									{if(flag) printf("MINUS\n"); col+=yyleng;}
"%"									{if(flag) printf("MOD\n"); col+=yyleng;}
"!="								{if(flag) printf("NE\n"); col+=yyleng;}
"!"									{if(flag) printf("NOT\n"); col+=yyleng;}
"||"								{if(flag) printf("OR\n"); col+=yyleng;}
"+"									{if(flag) printf("PLUS\n"); col+=yyleng;}
"}"									{if(flag) printf("RBRACE\n"); col+=yyleng;}
"return"							{if(flag) printf("RETURN\n"); col+=yyleng;}
")"									{if(flag) printf("RPAR\n"); col+=yyleng;}
"]"									{if(flag) printf("RSQ\n"); col+=yyleng;}
";"									{if(flag) printf("SEMI\n"); col+=yyleng;}
"void"								{if(flag) printf("VOID\n"); col+=yyleng;}
{reserved}							{if(flag) printf("RESERVED(%s)\n", yytext); col+=yyleng;}
{letter}({letter}|{number})*		{if(flag) printf("ID(%s)\n", yytext); col+=yyleng;}
{number}+							{if(flag) printf("INTLIT(%s)\n", yytext); col+=yyleng;}
\'([^\n\'\\]|{seq_escape})\'		{if(flag) printf("CHRLIT(%s)\n", yytext); col+=yyleng;}
\"([^\n\"\\]|{seq_escape})*\"		{if(flag) printf("STRLIT(%s)\n", yytext); col+=yyleng;}

\'([^\n\'\\]|\\.|\\[0-9]{1,3})*(\\)?	{printf("Line %d, col %d: unterminated char constant\n", line, col); col+=yyleng;}
\'([^\n\'\\]|\\.|\\[0-9]{1,3})*\'		{printf("Line %d, col %d: invalid char constant (%s)\n", line, col, yytext); col+=yyleng;}
\"([^\n\"\\]|\\.|\\[0-9]{1,3})*(\\)?	{printf("Line %d, col %d: unterminated string constant\n", line, col); col+=yyleng;}
\"([^\n\"\\]|\\.|\\[0-9]{1,3})*\"		{printf("Line %d, col %d: invalid string constant (%s)\n", line, col, yytext); col+=yyleng;}

" "|\t								{col+=yyleng;}
\n									{line++; col=1;}
.									{printf("Line %d, col %d: illegal character (%c)\n", line, col, yytext[0]); col+=yyleng;}
%%
int main(int argc, char* argv[]) {
	if(argc>1 && strcmp(argv[1],"-l") == 0)
		flag = 1;
	yylex();
	return 0;
}

int yywrap() {
	return 1;
}
