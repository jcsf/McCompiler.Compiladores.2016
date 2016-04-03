mccompiler: y.tab.c lex.yy.c ast.c
	cc -o mccompiler y.tab.c lex.yy.c ast.c -ll -ly

y.tab.c: mccompiler.y
	yacc -dv mccompiler.y

lex.yy.c: mccompiler.l
	lex mccompiler.l