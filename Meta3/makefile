mccompiler: y.tab.c lex.yy.c ast.c simbtab.c
	clang -o mccompiler y.tab.c lex.yy.c ast.c simbtab.c -ll -ly -Wall -Wno-unused-function -g

y.tab.c: mccompiler.y
	yacc -d mccompiler.y

lex.yy.c: mccompiler.l
	lex mccompiler.l