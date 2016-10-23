%{
	#include <stdio.h>
	#include <string.h>
	#include "ast.h"

	void yyerror(char *s);
	int yylex();

	extern int yyleng;
	extern char* yytext;

	extern int line;
	extern int col;
	extern int flag_error;

	node_type* root = NULL; 		/* this node represents the root of the AST */
	node_type* aux_node = NULL; 	/* auxiliary node used to build new nodes */
%}

%union {
	struct _token_struct* token;
	struct node_type* node;
}

%token <token> AMP AND ASSIGN AST CHAR COMMA DIV ELSE EQ FOR GE GT IF INT LBRACE LE
%token <token> LPAR LSQ LT MINUS MOD NE NOT OR PLUS RBRACE RETURN RPAR RSQ SEMI VOID
%token <token> RESERVED ID INTLIT CHRLIT STRLIT

%type <node> Start FunctionDefinition FunctionBody DeclarationList StatementList FunctionDeclaration FunctionDeclarator ParameterList
ParameterDeclaration Declaration DeclaratorList TypeSpec Declarator AstList ErrStatement Statement StatList ExprOpt GeneralExpr Expr ExprList

%left COMMA
%right ASSIGN
%left OR
%left AND
%left EQ NE
%left GT LT GE LE
%left PLUS MINUS
%left AST DIV MOD
%right NOT AMP
%left LPAR RPAR LSQ RSQ
%nonassoc IFX
%nonassoc ELSE

%%

Start:
	Start FunctionDefinition 												{aux_node = $1; aux_node = aux_node->children; add_sibling(aux_node,$2); $$ = $1;}
	| Start FunctionDeclaration 											{aux_node = $1; aux_node = aux_node->children; add_sibling(aux_node,$2); $$ = $1;}
	| Start Declaration 													{aux_node = $1; aux_node = aux_node->children; add_sibling(aux_node,$2); $$ = $1;}
	| FunctionDefinition 													{$$ = create_node("Program", NULL, -1, -1); add_child($$, $1); root = $$;}
	| FunctionDeclaration													{$$ = create_node("Program", NULL, -1, -1); add_child($$, $1); root = $$;}
	| Declaration 															{$$ = create_node("Program", NULL, -1, -1); add_child($$, $1); root = $$;}
	;
FunctionDefinition:
	TypeSpec FunctionDeclarator FunctionBody 								{aux_node = create_node("FuncDefinition", NULL, -1, -1); add_sibling($1,$2); add_sibling($1,$3); add_child(aux_node,$1); $$ = aux_node;}
	;
FunctionBody:
	LBRACE DeclarationList StatementList RBRACE 							{$$ = create_node("FuncBody", NULL, -1, -1); add_sibling($2,$3); add_child($$,$2);}
	| LBRACE DeclarationList RBRACE 										{$$ = create_node("FuncBody", NULL, -1, -1); add_child($$,$2);}
	| LBRACE StatementList RBRACE 											{$$ = create_node("FuncBody", NULL, -1, -1); if(is_null_node($2) == 0) {add_child($$,$2);}}
	| LBRACE RBRACE 														{$$ = create_node("FuncBody", NULL, -1, -1);}
	| LBRACE error RBRACE 													{$$ = create_node("Error", NULL, -1, -1);} /* an Error Node is created */
	;
DeclarationList:
	DeclarationList Declaration 											{add_sibling($1,$2); $$ = $1;}
	| Declaration 															{$$ = $1;}
	;
StatementList:
	StatementList ErrStatement 												{if(is_null_node($1)) {$$ = $2;} else if(is_null_node($2)) {$$ = $1;} else {add_sibling($1,$2); $$ = $1;}}
	| Statement 															{$$ = $1;}
	;
FunctionDeclaration:
	TypeSpec FunctionDeclarator SEMI 										{aux_node = create_node("FuncDeclaration", NULL, -1, -1); add_sibling($1,$2); add_child(aux_node,$1); $$ = aux_node;}
	;
FunctionDeclarator:
	AstList ID LPAR ParameterList RPAR 										{aux_node = create_node("ParamList", NULL, -1, -1); add_child(aux_node,$4); add_sibling($1,create_node("Id", $2->value, $2->line, $2->col)); add_sibling($1,aux_node); $$ = $1;}
	| ID LPAR ParameterList RPAR 											{$$ = create_node("Id", $1->value, $1->line, $1->col); aux_node = create_node("ParamList", NULL, -1, -1); add_child(aux_node,$3); add_sibling($$,aux_node);}
	;
ParameterList:
	ParameterList COMMA ParameterDeclaration 								{add_sibling($1,$3); $$ = $1;}
	| ParameterDeclaration 													{$$ = $1;}
	;
ParameterDeclaration:
	TypeSpec AstList ID 													{$$ = create_node("ParamDeclaration", NULL, -1, -1); aux_node = create_node("Id", $3->value, $3->line, $3->col); add_sibling($1,$2); add_sibling($1,aux_node); add_child($$,$1);}
	| TypeSpec ID 															{$$ = create_node("ParamDeclaration", NULL, -1, -1); aux_node = create_node("Id", $2->value, $2->line, $2->col); add_sibling($1,aux_node); add_child($$,$1);}
	| TypeSpec AstList 														{$$ = create_node("ParamDeclaration", NULL, -1, -1); add_sibling($1,$2); add_child($$,$1);}
	| TypeSpec 																{$$ = create_node("ParamDeclaration", NULL, -1, -1); add_child($$,$1);}
	;
Declaration:
	TypeSpec DeclaratorList SEMI 											{$$ = add_typespec_to_declarations($1,$2);}
	| error SEMI 															{$$ = create_node("Error", NULL, -1, -1);} /* an Error Node is created */
	;
DeclaratorList:
	DeclaratorList COMMA Declarator 										{add_sibling($1,$3); $$ = $1;}
	| Declarator 															{$$ = $1;}
	;
TypeSpec:
	CHAR 																	{$$ = create_node("Char", NULL, $1->line, $1->col);}
	| INT 																	{$$ = create_node("Int", NULL, $1->line, $1->col);}
	| VOID 																	{$$ = create_node("Void", NULL, $1->line, $1->col);}
	;
Declarator:
	AstList ID LSQ INTLIT RSQ 												{$$ = create_node("ArrayDeclaration", NULL, -1, -1); aux_node = create_node("Id", $2->value, $2->line, $2->col); add_sibling($1,aux_node); add_sibling($1,create_node("IntLit", $4->value, $4->line, $4->col)); add_child($$,$1);}
	| ID LSQ INTLIT RSQ 													{$$ = create_node("ArrayDeclaration", NULL, -1, -1); aux_node = create_node("Id", $1->value, $1->line, $1->col); add_sibling(aux_node,create_node("IntLit", $3->value, $3->line, $3->col)); add_child($$,aux_node);}
	| AstList ID 															{$$ = create_node("Declaration", NULL, -1, -1); aux_node = create_node("Id", $2->value, $2->line, $2->col); add_sibling($1,aux_node); add_child($$,$1);}
	| ID 																	{$$ = create_node("Declaration", NULL, -1, -1); aux_node = create_node("Id", $1->value, $1->line, $1->col); add_child($$,aux_node);}
	;
AstList:
	AstList AST 															{aux_node = create_node("Pointer", NULL, -1, -1); add_sibling($1,aux_node); $$ = $1;}
	| AST																	{$$ = create_node("Pointer", NULL, -1, -1);}
	;
ErrStatement:
	ExprOpt SEMI 															{$$ = $1;}

	| LBRACE StatList RBRACE 												{if(have_sibling($2) == 1){$$ = create_node("StatList", NULL, -1, -1); add_child($$,$2);} else {$$ = $2;}}
	| LBRACE RBRACE 														{$$ = create_node("Null", NULL, -1, -1);}
	| LBRACE error RBRACE 													{$$ = create_node("Error", NULL, -1, -1);} /* an Error Node is created */

	| IF LPAR GeneralExpr RPAR ErrStatement %prec IFX 						{$$ = create_node("If", NULL, $1->line, $1->col); aux_node = create_node("Null", NULL, -1, -1); add_sibling($3,$5); add_sibling($3,aux_node); add_child($$,$3);}
	| IF LPAR GeneralExpr RPAR ErrStatement ELSE ErrStatement 				{aux_node = create_node("If", NULL, $1->line, $1->col); add_sibling($3,$5); add_sibling($3,$7); add_child(aux_node,$3); $$ = aux_node;}

	| FOR LPAR ExprOpt SEMI ExprOpt SEMI ExprOpt RPAR ErrStatement 			{aux_node = create_node("For", NULL, $1->line, $1->col); add_sibling($3,$5); add_sibling($3,$7); add_sibling($3,$9); add_child(aux_node,$3); $$ = aux_node;}

	| RETURN ExprOpt SEMI 													{aux_node = create_node("Return", NULL, $2->token_line, $2->token_col); add_child(aux_node,$2); $$ = aux_node;}

	| error SEMI 															{$$ = create_node("Error", NULL, -1, -1);} /* an Error Node is created */
	;
Statement:
	ExprOpt SEMI 															{$$ = $1;}

	| LBRACE StatList RBRACE 												{if(have_sibling($2) == 1){$$ = create_node("StatList", NULL, -1, -1); add_child($$,$2);} else {$$ = $2;}}
	| LBRACE RBRACE 														{$$ = create_node("Null", NULL, -1, -1);}
	| LBRACE error RBRACE 													{$$ = create_node("Error", NULL, -1, -1);} /* an Error Node is created */

	| IF LPAR GeneralExpr RPAR ErrStatement %prec IFX 						{$$ = create_node("If", NULL, $1->line, $1->col); aux_node = create_node("Null", NULL, -1, -1); add_sibling($3,$5); add_sibling($3,aux_node); add_child($$,$3);}
	| IF LPAR GeneralExpr RPAR ErrStatement ELSE ErrStatement 				{aux_node = create_node("If", NULL, $1->line, $1->col); add_sibling($3,$5); add_sibling($3,$7); add_child(aux_node,$3); $$ = aux_node;}

	| FOR LPAR ExprOpt SEMI ExprOpt SEMI ExprOpt RPAR ErrStatement 			{aux_node = create_node("For", NULL, $1->line, $1->col); add_sibling($3,$5); add_sibling($3,$7); add_sibling($3,$9); add_child(aux_node,$3); $$ = aux_node;}

	| RETURN ExprOpt SEMI 													{aux_node = create_node("Return", NULL, $1->line, $1->col); add_child(aux_node,$2); $$ = aux_node;}
	;
StatList:
	StatList ErrStatement 													{if(is_null_node($1)) {$$ = $2;} else if(is_null_node($2)) {$$ = $1;} else {add_sibling($1,$2); $$ = $1;}}
	| ErrStatement 															{$$ = $1;}
	;
ExprOpt:
	GeneralExpr 															{$$ = $1;}
	| 																		{$$ = create_node("Null", NULL, -1, -1);}
	;
GeneralExpr:
	GeneralExpr COMMA Expr 													{aux_node = create_node("Comma", NULL, $2->line, $2->col); add_sibling($1,$3); add_child(aux_node,$1); $$ = aux_node;}
	| Expr 																	{$$ = $1;}
	;
Expr:
	Expr ASSIGN Expr 														{aux_node = create_node("Store", NULL, $2->line, $2->col); add_sibling($1,$3); add_child(aux_node,$1); $$ = aux_node;}

	| Expr AND Expr 														{aux_node = create_node("And", NULL, $2->line, $2->col); add_sibling($1,$3); add_child(aux_node,$1); $$ = aux_node;}
	| Expr OR Expr 															{aux_node = create_node("Or", NULL, $2->line, $2->col); add_sibling($1,$3); add_child(aux_node,$1); $$ = aux_node;}

	| Expr EQ Expr 															{aux_node = create_node("Eq", NULL, $2->line, $2->col); add_sibling($1,$3); add_child(aux_node,$1); $$ = aux_node;}
	| Expr NE Expr 															{aux_node = create_node("Ne", NULL, $2->line, $2->col); add_sibling($1,$3); add_child(aux_node,$1); $$ = aux_node;}
	| Expr LT Expr 															{aux_node = create_node("Lt", NULL, $2->line, $2->col); add_sibling($1,$3); add_child(aux_node,$1); $$ = aux_node;}
	| Expr GT Expr 															{aux_node = create_node("Gt", NULL, $2->line, $2->col); add_sibling($1,$3); add_child(aux_node,$1); $$ = aux_node;}
	| Expr LE Expr 															{aux_node = create_node("Le", NULL, $2->line, $2->col); add_sibling($1,$3); add_child(aux_node,$1); $$ = aux_node;}
	| Expr GE Expr 															{aux_node = create_node("Ge", NULL, $2->line, $2->col); add_sibling($1,$3); add_child(aux_node,$1); $$ = aux_node;}

	| Expr PLUS Expr 														{aux_node = create_node("Add", NULL, $2->line, $2->col); add_sibling($1,$3); add_child(aux_node,$1); $$ = aux_node;}
	| Expr MINUS Expr 														{aux_node = create_node("Sub", NULL, $2->line, $2->col); add_sibling($1,$3); add_child(aux_node,$1); $$ = aux_node;}
	| Expr AST Expr 														{aux_node = create_node("Mul", NULL, $2->line, $2->col); add_sibling($1,$3); add_child(aux_node,$1); $$ = aux_node;}
	| Expr DIV Expr 														{aux_node = create_node("Div", NULL, $2->line, $2->col); add_sibling($1,$3); add_child(aux_node,$1); $$ = aux_node;}
	| Expr MOD Expr 														{aux_node = create_node("Mod", NULL, $2->line, $2->col); add_sibling($1,$3); add_child(aux_node,$1); $$ = aux_node;}

	| AMP Expr 																{$$ = create_node("Addr", NULL, $1->line, $1->col); add_child($$,$2);}
	| AST Expr %prec NOT													{$$ = create_node("Deref", NULL, $1->line, $1->col); add_child($$,$2);}
	| PLUS Expr %prec NOT													{$$ = create_node("Plus", NULL, $1->line, $1->col); add_child($$,$2);}
	| MINUS Expr %prec NOT													{$$ = create_node("Minus", NULL, $1->line, $1->col); add_child($$,$2);}
	| NOT Expr 																{$$ = create_node("Not", NULL, $1->line, $1->col); add_child($$,$2);}

	| Expr LSQ GeneralExpr RSQ 												{$$ = create_node("Deref", strdup("array"), $2->line, $2->col); aux_node = create_node("Add", NULL, $1->token_line, $1->token_col); add_sibling($1,$3); add_child(aux_node,$1); add_child($$,aux_node);}

	| ID LPAR ExprList RPAR 												{$$ = create_node("Call", NULL, $1->line, $1->col); aux_node = create_node("Id", $1->value, $1->line, $1->col); add_sibling(aux_node,$3); add_child($$,aux_node);}
	| ID LPAR RPAR 															{$$ = create_node("Call", NULL, $1->line, $1->col); aux_node = create_node("Id", $1->value, $1->line, $1->col); add_child($$,aux_node);}
	| ID LPAR error RPAR 													{$$ = create_node("Error", NULL, -1, -1);} /* an Error Node is created */
	
	| ID 																	{$$ = create_node("Id", $1->value, $1->line, $1->col);}
	| INTLIT 																{$$ = create_node("IntLit", $1->value, $1->line, $1->col);}
	| CHRLIT 																{$$ = create_node("ChrLit", $1->value, $1->line, $1->col);}
	| STRLIT 																{$$ = create_node("StrLit", $1->value, $1->line, $1->col);}
	| LPAR GeneralExpr RPAR													{$$ = $2;}
	| LPAR error RPAR														{$$ = create_node("Error", NULL, -1, -1);} /* an Error Node is created */
	;
ExprList:
	ExprList COMMA Expr 													{add_sibling($1,$3); $$ = $1;}
	| Expr 																	{$$ = $1;}
	;

%%

void yyerror(char *s) {
	flag_error = 1;
	printf("Line %d, col %d: %s: %s\n", line, col-yyleng, s, yytext);
}