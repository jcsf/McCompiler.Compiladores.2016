#ifndef AST_TO_LLVM_H_INCLUDED
#define AST_TO_LLVM_H_INCLUDED

typedef enum {Func_Definition, Func_Declaration} Func_Header_Type;

extern symbol_node* root_st;

void ast_to_llvm(node_type*, symbol_node*);

void llvm_function_header(node_type*, symbol_node*, Func_Header_Type);

void llvm_declaration(node_type*, symbol_node*);

void llvm_array_declaration(node_type*, symbol_node*);

void llvm_strlit(node_type*);

void llvm_add(node_type*);

void llvm_sub(node_type*);

void llvm_multiplicative(node_type*);

void llvm_relational(node_type*);

void llvm_equality(node_type*);

void llvm_not(node_type*);

void llvm_and_or(node_type*, symbol_node*);

void llvm_deref(node_type*);

void llvm_call(node_type*, symbol_node*);

void llvm_store(node_type*, symbol_node*);

void llvm_if(node_type*, symbol_node*);

void llvm_for(node_type*, symbol_node*);

char* convert_c_type_to_llvm_type(char*);

char* get_size_from_annotation(char*);

void get_strlit_to_llvm(node_type*);

char* parsing_strlit(char*, int);

void print_hex(const char *s);

char search_symbol_local(char*, symbol_node*);

void cast_llvm_types(node_type*, char*);

int number_arg(char*, symbol_node*);

#endif // AST_TO_LLVM_H_INCLUDED