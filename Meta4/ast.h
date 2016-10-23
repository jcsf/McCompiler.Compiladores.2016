#ifndef AST_H_INCLUDED
#define AST_H_INCLUDED

/* Structure used between LEX and YACC */
typedef struct _token_struct {
	char* value;
	int line;
	int col;
} token_struct;


/* Structure that represents an AST node */
typedef struct node_type {
	char* type;
	char* token;

	int token_line;
	int token_col;
	char* annotation;

	char* llvm_var;
	int label;

	struct node_type* children;
	struct node_type* siblings;
} node_type;


node_type* create_node(char*, char*, int, int);

void add_child(node_type*, node_type*);

void add_sibling(node_type*, node_type*);

int have_sibling(node_type*);

int is_null_node(node_type*);

node_type* add_typespec_to_declarations(node_type*, node_type*);

void print_ast(node_type*, int);

void clear_ast(node_type*);

#endif // AST_H_INCLUDED