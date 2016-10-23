#ifndef SIMBTAB_H_INCLUDED
#define SIMBTAB_H_INCLUDED

typedef enum {Global, Func, Var} Symbol_Type;

typedef struct symbol_node {
	Symbol_Type tipo;
	
	char* id;
	char* type;
	
	union {
		int flag_print;
		int flag_param;
	} flag;
	
	struct symbol_node* next;
	struct symbol_node* children;

} symbol_node;

symbol_node* ast_to_symbol_table(node_type*, symbol_node*);

symbol_node* create_symb(Symbol_Type, char*, char*, int);

void add_symbol_to_table(symbol_node*, symbol_node*);

void create_declaration_and_array_declaration(node_type*, symbol_node*, int);

void create_param_declarations_from_param_list(node_type*, symbol_node*);

void create_func_table(node_type*, symbol_node*, int);

void print_symbol_table(symbol_node*);

char* create_func_header(symbol_node*);

char* search_symbol_type(node_type*, symbol_node*);

symbol_node* search_symbol_table(char*, symbol_node*);

char* search_func_type(char*); /* NOT USED */

char* check_store_op(node_type*);

char* check_not_op(node_type*);

char* check_relational_op(node_type*);

char* check_equality_op(node_type*);

char* check_logical_op(node_type*);

char* check_multiplicative_op(node_type*);

char* check_comma_op(node_type*);

char* check_unary_op(node_type*);

char* check_add_op(node_type*, int);

char* check_sub_op(node_type*);

char* check_deref_op(node_type*);

char* check_deref_array_op(node_type*);

char* check_addr_op(node_type*);

void check_if(node_type*);

void check_for(node_type*);

int is_pointer_to_object(char*);

int is_pointer(char*);

int check_func_errors(symbol_node*, symbol_node*, int, int);

char* check_call(node_type*, symbol_node*);

void check_return(node_type*, symbol_node*);

int check_compatibility(node_type*, char*);

int strlit_length(char*);

int num_length(int);

int n_params_on_func(symbol_node*);

void change_array_to_pointer(char*);

int search_l_value(char*, symbol_node*); /* NOT USED */

void clear_symbol_table(symbol_node*);

void clear_func_table(symbol_node*);

void clear_root_st();

#endif // SIMBTAB_H_INCLUDED