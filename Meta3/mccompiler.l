%X COMMENT

%{
	#include "y.tab.h"
	#include "ast.h"
	#include "simbtab.h"

	int line = 1;
	int col = 1;
	int start_line = 1;
	int start_col = 1;
	
	int flag_l = 0;
	int flag_1 = 0;
	int flag_t = 0;
	int flag_2 = 0;
	int flag_s = 0;
	int flag_error = 0;

	extern node_type* root;
	symbol_node* sym_tab;

	#define CREATE_TOKEN() {token_struct* temp_token = (token_struct*) malloc(sizeof(struct _token_struct)); temp_token->value = strdup(yytext); temp_token->line = line; temp_token->col = col; yylval.token = temp_token;}
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

"&"									{CREATE_TOKEN(); if(flag_l) printf("AMP\n"); col+=yyleng; if(!flag_1) return AMP;}
"&&"								{CREATE_TOKEN(); if(flag_l) printf("AND\n"); col+=yyleng; if(!flag_1) return AND;}
"="									{CREATE_TOKEN(); if(flag_l) printf("ASSIGN\n"); col+=yyleng; if(!flag_1) return ASSIGN;}
"*"									{CREATE_TOKEN(); if(flag_l) printf("AST\n"); col+=yyleng; if(!flag_1) return AST;}
"char"								{CREATE_TOKEN(); if(flag_l) printf("CHAR\n"); col+=yyleng; if(!flag_1) return CHAR;}
","									{CREATE_TOKEN(); if(flag_l) printf("COMMA\n"); col+=yyleng; if(!flag_1) return COMMA;}
"/"									{CREATE_TOKEN(); if(flag_l) printf("DIV\n"); col+=yyleng; if(!flag_1) return DIV;}
"else"								{CREATE_TOKEN(); if(flag_l) printf("ELSE\n"); col+=yyleng; if(!flag_1) return ELSE;}
"=="								{CREATE_TOKEN(); if(flag_l) printf("EQ\n"); col+=yyleng; if(!flag_1) return EQ;}
"for"								{CREATE_TOKEN(); if(flag_l) printf("FOR\n"); col+=yyleng; if(!flag_1) return FOR;}
">="								{CREATE_TOKEN(); if(flag_l) printf("GE\n"); col+=yyleng; if(!flag_1) return GE;}
">"									{CREATE_TOKEN(); if(flag_l) printf("GT\n"); col+=yyleng; if(!flag_1) return GT;}
"if"								{CREATE_TOKEN(); if(flag_l) printf("IF\n"); col+=yyleng; if(!flag_1) return IF;}
"int"								{CREATE_TOKEN(); if(flag_l) printf("INT\n"); col+=yyleng; if(!flag_1) return INT;}
"{"									{CREATE_TOKEN(); if(flag_l) printf("LBRACE\n"); col+=yyleng; if(!flag_1) return LBRACE;}
"<="								{CREATE_TOKEN(); if(flag_l) printf("LE\n"); col+=yyleng; if(!flag_1) return LE;}
"("									{CREATE_TOKEN(); if(flag_l) printf("LPAR\n"); col+=yyleng; if(!flag_1) return LPAR;}
"["									{CREATE_TOKEN(); if(flag_l) printf("LSQ\n"); col+=yyleng; if(!flag_1) return LSQ;}
"<"									{CREATE_TOKEN(); if(flag_l) printf("LT\n"); col+=yyleng; if(!flag_1) return LT;}
"-"									{CREATE_TOKEN(); if(flag_l) printf("MINUS\n"); col+=yyleng; if(!flag_1) return MINUS;}
"%"									{CREATE_TOKEN(); if(flag_l) printf("MOD\n"); col+=yyleng; if(!flag_1) return MOD;}
"!="								{CREATE_TOKEN(); if(flag_l) printf("NE\n"); col+=yyleng; if(!flag_1) return NE;}
"!"									{CREATE_TOKEN(); if(flag_l) printf("NOT\n"); col+=yyleng; if(!flag_1) return NOT;}
"||"								{CREATE_TOKEN(); if(flag_l) printf("OR\n"); col+=yyleng; if(!flag_1) return OR;}
"+"									{CREATE_TOKEN(); if(flag_l) printf("PLUS\n"); col+=yyleng; if(!flag_1) return PLUS;}
"}"									{CREATE_TOKEN(); if(flag_l) printf("RBRACE\n"); col+=yyleng; if(!flag_1) return RBRACE;}
"return"							{CREATE_TOKEN(); if(flag_l) printf("RETURN\n"); col+=yyleng; if(!flag_1) return RETURN;}
")"									{CREATE_TOKEN(); if(flag_l) printf("RPAR\n"); col+=yyleng; if(!flag_1) return RPAR;}
"]"									{CREATE_TOKEN(); if(flag_l) printf("RSQ\n"); col+=yyleng; if(!flag_1) return RSQ;}
";"									{CREATE_TOKEN(); if(flag_l) printf("SEMI\n"); col+=yyleng; if(!flag_1) return SEMI;}
"void"								{CREATE_TOKEN(); if(flag_l) printf("VOID\n"); col+=yyleng; if(!flag_1) return VOID;}

{reserved}							{CREATE_TOKEN(); if(flag_l) printf("RESERVED(%s)\n", yytext); col+=yyleng; if(!flag_1) return RESERVED;}
{letter}({letter}|{number})*		{CREATE_TOKEN(); if(flag_l) printf("ID(%s)\n", yytext); col+=yyleng; if(!flag_1) return ID;}
{number}+							{CREATE_TOKEN(); if(flag_l) printf("INTLIT(%s)\n", yytext); col+=yyleng; if(!flag_1) return INTLIT;}
\'([^\n\'\\]|{seq_escape})\'		{CREATE_TOKEN(); if(flag_l) printf("CHRLIT(%s)\n", yytext); col+=yyleng; if(!flag_1) return CHRLIT;}
\"([^\n\"\\]|{seq_escape})*\"		{CREATE_TOKEN(); if(flag_l) printf("STRLIT(%s)\n", yytext); col+=yyleng; if(!flag_1) return STRLIT;}

\'([^\n\'\\]|\\.|\\[0-9]{1,3})*(\\)?	{printf("Line %d, col %d: unterminated char constant\n", line, col); col+=yyleng;}
\'([^\n\'\\]|\\.|\\[0-9]{1,3})*\'		{printf("Line %d, col %d: invalid char constant (%s)\n", line, col, yytext); col+=yyleng;}
\"([^\n\"\\]|\\.|\\[0-9]{1,3})*(\\)?	{printf("Line %d, col %d: unterminated string constant\n", line, col); col+=yyleng;}
\"([^\n\"\\]|\\.|\\[0-9]{1,3})*\"		{printf("Line %d, col %d: invalid string constant (%s)\n", line, col, yytext); col+=yyleng;}

" "|\t								{col+=yyleng;}
<<EOF>>								{col+=yyleng; return yytext[0];}
\n									{line++; col=1;}
.									{printf("Line %d, col %d: illegal character (%c)\n", line, col, yytext[0]); col+=yyleng;}

%%

int main(int argc, char* argv[]) {
	if(argc > 1) {
		/* ---META 1--- */
		if(strcmp(argv[1], "-l") == 0) {
			flag_l = 1;
			flag_1 = 1;
		}

		if(strcmp(argv[1], "-1") == 0)
			flag_1 = 1;

		/* ---META 2--- */
		if(strcmp(argv[1], "-t") == 0) {
			flag_t = 1;
			flag_2 = 1;
		}

		if(strcmp(argv[1], "-2") == 0)
			flag_2 = 1;

		/* ---META 3--- */
		if(strcmp(argv[1], "-s") == 0)
			flag_s = 1;
	}

	if(flag_1) {
		yylex();
	} else if (flag_2){
		yyparse();

		if(flag_t && !flag_error)
			print_ast(root, 0);

		clear_ast(root);
	} else {
		yyparse();
		
		if(!flag_error) {
			sym_tab = ast_to_symbol_table(root, NULL);
			
			if(flag_s) {
				print_symbol_table(sym_tab);
				print_ast(root, 0);
			}

			clear_symbol_table(sym_tab);
		}
		
		clear_ast(root);	
	}

	return 0;
}

int yywrap() {
	return 1;
}
