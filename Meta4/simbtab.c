#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ast.h"
#include "simbtab.h"

symbol_node* root_st = NULL; /* pointer to the Global Symbol Table */
int param_without_id = 0;
int void_param_error;

symbol_node* ast_to_symbol_table(node_type* ast, symbol_node* table) {
	symbol_node* aux_t;
	int temp;

	if(ast != NULL) {

		if(strcmp(ast->type,"Program") == 0) {

			table = create_symb(Global, "Global", NULL, 1);

			root_st = table; /* to always grant access to the Global Symbol Table */
			
			/* CREATE TAB ATOI */
			aux_t = create_symb(Func, "atoi", "int", 0);
			add_symbol_to_table(aux_t, create_symb(Var, "a", "char*", 1));
			aux_t->type = create_func_header(aux_t);
			add_symbol_to_table(table, aux_t);

			/* CREATE TAB ITOA */
			aux_t = create_symb(Func, "itoa", "char*", 0);
			add_symbol_to_table(aux_t, create_symb(Var, "a","int", 1));
			add_symbol_to_table(aux_t, create_symb(Var, "b","char*", 1));
			aux_t->type = create_func_header(aux_t);
			add_symbol_to_table(table, aux_t);

			/* CREATE TAB PUTS */
			aux_t = create_symb(Func, "puts", "int", 0);
			add_symbol_to_table(aux_t, create_symb(Var, "a","char*", 1));
			aux_t->type = create_func_header(aux_t);
			add_symbol_to_table(table, aux_t);

			ast_to_symbol_table(ast->children, table);
			
			return table;
		
		} else if (strcmp(ast->type,"ArrayDeclaration") == 0 || strcmp(ast->type,"Declaration") == 0) {
			create_declaration_and_array_declaration(ast->children, table, 0);

		} else if (strcmp(ast->type,"ParamList") == 0) {
			create_param_declarations_from_param_list(ast->children, table);

		} else if (strcmp(ast->type,"FuncDeclaration") == 0) {
			create_func_table(ast->children, table, 0);

		} else if (strcmp(ast->type,"FuncDefinition") == 0) {
			create_func_table(ast->children, table, 1);

		} else if (strcmp(ast->type,"FuncBody") == 0) {
			ast_to_symbol_table(ast->children, table);
			
		} else if(strcmp(ast->type,"IntLit") == 0) {
			ast->annotation = strdup("int");

		} else if(strcmp(ast->type,"ChrLit") == 0) {
			ast->annotation = strdup("int");

		} else if(strcmp(ast->type,"StrLit") == 0) {
			temp = strlit_length(ast->token);
			ast->annotation = (char*) malloc ((7+num_length(temp)) * sizeof(char));
			sprintf(ast->annotation, "char[%d]", temp);

		} else if(strcmp(ast->type,"Id") == 0) {
			ast->annotation = strdup(search_symbol_type(ast, table));

		} else if(strcmp(ast->type,"Call") == 0) {
			ast_to_symbol_table(ast->children, table);
			ast->annotation = check_call(ast, table);

		} else if(strcmp(ast->type,"Store") == 0) {
			ast_to_symbol_table(ast->children, table);
			ast->annotation = check_store_op(ast);

		} else if(strcmp(ast->type, "Not") == 0) {
			ast_to_symbol_table(ast->children, table);
			ast->annotation = check_not_op(ast);

		} else if(strcmp(ast->type, "Lt") == 0 || strcmp(ast->type, "Gt") == 0 || strcmp(ast->type, "Le") == 0 || strcmp(ast->type, "Ge") == 0) {
			ast_to_symbol_table(ast->children, table);
			ast->annotation = check_relational_op(ast);

		} else if(strcmp(ast->type, "Eq") == 0 || strcmp(ast->type, "Ne") == 0) {
			ast_to_symbol_table(ast->children, table);
			ast->annotation = check_equality_op(ast);

		} else if(strcmp(ast->type, "Or") == 0 || strcmp(ast->type, "And") == 0) {
			ast_to_symbol_table(ast->children, table);
			ast->annotation = check_logical_op(ast);

		} else if(strcmp(ast->type, "Mul") == 0 || strcmp(ast->type, "Div") == 0 || strcmp(ast->type, "Mod") == 0) {
			ast_to_symbol_table(ast->children, table);
			ast->annotation = check_multiplicative_op(ast);

		} else if(strcmp(ast->type, "Comma") == 0) {
			ast_to_symbol_table(ast->children, table);
			ast->annotation = check_comma_op(ast);

		} else if(strcmp(ast->type, "Plus") == 0 || strcmp(ast->type, "Minus") == 0) {
			ast_to_symbol_table(ast->children, table);
			ast->annotation = check_unary_op(ast);

		} else if(strcmp(ast->type, "Add") == 0) {
			ast_to_symbol_table(ast->children, table);
			ast->annotation = check_add_op(ast, 0);

		} else if(strcmp(ast->type, "Sub") == 0) {
			ast_to_symbol_table(ast->children, table);
			ast->annotation = check_sub_op(ast);

		} else if(strcmp(ast->type, "Deref") == 0) {
			if(ast->token != NULL) {
				ast_to_symbol_table((ast->children)->children, table);
				ast->annotation = check_deref_array_op(ast);
			} else {
				ast_to_symbol_table(ast->children, table);
				ast->annotation = check_deref_op(ast);
			}

		} else if(strcmp(ast->type, "Addr") == 0) {
			ast_to_symbol_table(ast->children, table);
			ast->annotation = check_addr_op(ast);
		
		} else if(strcmp(ast->type, "If") == 0) {
			ast_to_symbol_table(ast->children, table);
			check_if(ast);

		} else if(strcmp(ast->type, "For") == 0) {
			ast_to_symbol_table(ast->children, table);
			check_for(ast);

		} else if(strcmp(ast->type, "Return") == 0) {
			ast_to_symbol_table(ast->children, table);
			check_return(ast, table);
			
		} else {
			ast_to_symbol_table(ast->children, table);

		}

		ast_to_symbol_table(ast->siblings, table);
	}

	return NULL;
}

symbol_node* create_symb(Symbol_Type t, char* id, char* type, int flag) {
	symbol_node* new_symbol = (symbol_node*) malloc(sizeof(symbol_node));
	char null_name [100];

	/* SET POINTERS TO NULL */
	new_symbol->next = NULL;
	new_symbol->children = NULL;
	new_symbol->type = NULL;
	new_symbol->id = NULL;
	
	new_symbol->tipo = t;
	if (id != NULL) {
		new_symbol->id = strdup(id);
	} else {
		sprintf(null_name, "%d", param_without_id);
		new_symbol->id = strdup(null_name);
		param_without_id++;
	}

	if(new_symbol->tipo == Func) {
		new_symbol->flag.flag_print = flag;
		new_symbol->children = create_symb(Var, "return", type, 0);
	} else if (new_symbol->tipo == Var) {
		new_symbol->flag.flag_param = flag;
		new_symbol->type = strdup(type);
		new_symbol->children = NULL;
	} else if (new_symbol->tipo == Global){
		new_symbol->type = NULL;
		new_symbol->children = NULL;
	} else {
		free(new_symbol->id);
		free(new_symbol);
		printf("ERROR: TIPO DE SIMBOLO INVÁLIDO!\n");
		new_symbol = NULL;
	}

	return new_symbol;
}

void add_symbol_to_table(symbol_node* tab, symbol_node* new_simb) {
	if(tab != NULL) {
		if(tab->tipo == Global) {
			while(tab->next != NULL)
				tab = tab->next;
			tab->next = new_simb;
		} else if (tab->tipo == Func) {
			while(tab->children != NULL)
				tab = tab->children;
			tab->children = new_simb;
		}
	}
}

void create_declaration_and_array_declaration(node_type* node, symbol_node* table, int flag_param) {
	char *id;
	char buffer[1024];
	symbol_node* local_table;
	int line, col, line_decl, col_decl, have_id = 0, num_params;
	unsigned int len;
	Symbol_Type t_table;
	node_type* param_list;

	if(flag_param == 1) {
		param_list = node;
		node = node->children;
	}

	while (node != NULL) {
		if(strcmp(node->type,"Id") == 0) {
			id = node->token;
			line = node->token_line;
			col = node->token_col;
			have_id = 1;
		} else if (strcmp(node->type,"IntLit") == 0){
			if(node->token[0] == '0') {
				sscanf(node->token,"%o", &len);
			} else {
				len = atoi(node->token);
			}
			sprintf(buffer + strlen(buffer), "[%d]", len);
		} else if (strcmp(node->type,"Pointer") == 0){
			sprintf(buffer + strlen(buffer), "*");
		} else {
			if (strcmp(node->type,"Int") == 0) {
				sprintf(buffer, "int");
			} else if (strcmp(node->type,"Char") == 0) {
				sprintf(buffer, "char");
			} else if (strcmp(node->type,"Void") == 0) {
				sprintf(buffer, "void");
				line_decl = node->token_line;
				col_decl = node->token_col;
			}
		}
		
		node = node->siblings;
	}

	if(strcmp(buffer, "void") == 0) {
		if(flag_param == 0) {
			printf("Line %d, col %d: Invalid use of void type in declaration\n", line_decl, col_decl);
			return;
		} else {
			num_params = n_params_on_func(table);
			if (have_id == 1) {
				if (void_param_error == 0) {
					printf("Line %d, col %d: Invalid use of void type in declaration\n", line_decl, col_decl);
					void_param_error = 1;
				}
				add_symbol_to_table(table, create_symb(Var, id, buffer, flag_param));
				return;
			} else if (param_list->siblings != NULL || num_params > 0) {
				if (void_param_error == 0) {
					printf("Line %d, col %d: Invalid use of void type in declaration\n", line_decl, col_decl);
					void_param_error = 1;
				}
				add_symbol_to_table(table, create_symb(Var, NULL, buffer, flag_param));
				return;
			}
		}
	}

	if(have_id == 1) {
		t_table = table->tipo;
		
		if(t_table != Global) { 
			local_table = table->children;
		} else {
			local_table = table;
		}
		
		while(local_table != NULL) {
			if (t_table == Global && strcmp(id, local_table->id) == 0) {
				if (strcmp(buffer, local_table->type) != 0)
					printf("Line %d, col %d: Conflicting types (got %s, expected %s)\n", line, col, buffer, local_table->type);
				return;
			} else if(strcmp(id, local_table->id) == 0) {
				printf("Line %d, col %d: Symbol %s already defined\n", line, col, id);
				if(flag_param == 1) {
					add_symbol_to_table(table, create_symb(Var, NULL, buffer, flag_param));
				}
				return;
			}

			if(t_table == Func) {
				local_table = local_table->children;	
			} else {
				local_table = local_table->next;
			}
				
		}

		add_symbol_to_table(table, create_symb(Var, id, buffer, flag_param));
	} else {
		add_symbol_to_table(table, create_symb(Var, NULL, buffer, flag_param));
	}
}

void create_param_declarations_from_param_list(node_type* param_decl, symbol_node* table) {

	void_param_error = 0;

	while(param_decl != NULL) {
		create_declaration_and_array_declaration(param_decl, table, 1);
		param_decl = param_decl->siblings;
	}

	void_param_error = 0;

	/* ADD TYPE TO FUNCTION */
	table->type = create_func_header(table);
}

void create_func_table(node_type* node, symbol_node* table, int flag_print) {
	char *id;
	char buffer[1024];
	int find_void, find_others, line, col, line_decl, col_decl;
	symbol_node *func_table, *temp_table, *old_node;

	while (strcmp(node->type,"ParamList") != 0) {
		if(strcmp(node->type,"Id") == 0) {
			id = node->token;
			line = node->token_line;
			col = node->token_col;
		} else if (strcmp(node->type,"Pointer") == 0){
			sprintf(buffer + strlen(buffer), "*");
		} else {
			if (strcmp(node->type,"Int") == 0) {
				sprintf(buffer, "int");
				line_decl = node->token_line;
				col_decl = node->token_col;
			} else if (strcmp(node->type,"Char") == 0) {
				sprintf(buffer, "char");
				line_decl = node->token_line;
				col_decl = node->token_col;
			} else if (strcmp(node->type,"Void") == 0) {
				sprintf(buffer, "void");
				line_decl = node->token_line;
				col_decl = node->token_col;
			}
		}
			
		node = node->siblings;
	}

	func_table = create_symb(Func, id, buffer, flag_print);
	ast_to_symbol_table(node, func_table);
	
	/* CHECK FOR VOID PARAMS */
	temp_table = old_node = func_table;
	find_void = 0; find_others = 0;
	while(temp_table != NULL) {
		if(temp_table->tipo == Var && temp_table->flag.flag_param == 1 && strcmp(temp_table->type,"void") == 0) {
			if(isdigit(temp_table -> id[0])) {
				find_void++;
			} else {
				find_void = 2;
			}
			old_node->children = temp_table->children;
			free(temp_table->type);
			free(temp_table->id);
			free(temp_table);
			temp_table = old_node->children;
		} else if (temp_table->tipo == Var && temp_table->flag.flag_param == 1){
			find_others++;
			old_node = temp_table;
			temp_table = temp_table->children;
		} else {
			old_node = temp_table;
			temp_table = temp_table->children;
		}	
	}

	if(find_void > 1 || (find_others > 0 && find_void > 0)) {
		/*printf("Line %d, col %d: Invalid use of void type in declaration\n", line, col);*/
		clear_func_table(func_table);
		return;
	}

	/* CHECK FOR A FUNCTION DECLARATION */
	temp_table = table;

	while (temp_table != NULL && strcmp(func_table->id, temp_table->id) != 0) {
		old_node = temp_table;
		temp_table = temp_table->next;	
	}

	if(temp_table != NULL) { /* IF EXISTS A DECLARATION */
		if(temp_table->flag.flag_print == 0 || func_table->flag.flag_print == 0) {
			if(check_func_errors(temp_table, func_table, line, col)) {
				old_node->next = func_table;
				func_table->next = temp_table->next;
				clear_func_table(temp_table);
			} else {
				clear_func_table(func_table);
			}
		} else { /* DEFINITION DOUBLED */
			printf("Line %d, col %d: Symbol %s already defined\n", line, col, func_table->id);
			clear_func_table(func_table);
		}
	} else {
		add_symbol_to_table(table, func_table);
	}
}

void print_symbol_table(symbol_node* tab_symb) {
	symbol_node* tab;

	while (tab_symb != NULL) {
		tab = tab_symb;
		
		if(tab->tipo == Global) {
			printf("===== Global Symbol Table =====\n");
			while(tab != NULL) {
				if (tab->tipo == Func || tab->tipo == Var){
					printf("%s\t%s\n", tab->id, tab->type);
				}
				tab = tab->next;
			}
			printf("\n");
		} else if (tab->tipo == Func && tab->flag.flag_print == 1) {
			printf("===== Function %s Symbol Table =====\n", tab->id);
			tab = tab->children;
			while(tab != NULL) {
				if(!isdigit(tab->id[0])) {
					if (tab->tipo == Var && tab->flag.flag_param == 1) {
						printf("%s\t%s\tparam\n", tab->id, tab->type);
					} else {
						printf("%s\t%s\n", tab->id, tab->type);
					}
				}
				tab = tab->children;
			}
			printf("\n");
		}

		tab_symb = tab_symb->next;
	}
}

char* create_func_header(symbol_node* f) {
	char buffer[1024], buf[1024];
	int count = 0;
	symbol_node* temp = f;

	while(temp != NULL) {
		if(temp->tipo == Var && temp->flag.flag_param == 1) {
			if (count == 0) {
				sprintf(buf, "%s", temp->type);
				count = 1;
			} else {
				sprintf(buf + strlen(buf), ",%s", temp->type);
			}
		}
		temp = temp->children;
	}
	
	sprintf(buffer, "%s(%s)", (f->children)->type, buf);

	return strdup(buffer);
}

char* search_symbol_type(node_type* node, symbol_node* local_table) {
	int i;
	symbol_node* recursive = NULL;

	if(local_table->tipo == Global) {
		i = 1;
	} else {
		recursive = local_table;
		local_table = local_table->children;
		i = 0;
	}

	for (; i < 2; i++) {

		while(local_table != NULL) {
			if(strcmp(local_table->id, node->token) == 0) {
				return local_table->type;
			}
			if(i == 0) {
				local_table = local_table->children;
			} else {
				local_table = local_table->next;
			}
		}

		local_table = root_st;
	}

	if (recursive != NULL && strcmp(recursive->id, node->token) == 0) {
		return recursive->type;
	}
	printf("Line %d, col %d: Unknown symbol %s\n", node->token_line, node->token_col, node->token);
	return strdup("undef");
}

symbol_node* search_symbol_table(char* symbol_id, symbol_node* local_table) {
	int i;

	if(local_table->tipo == Global) {
		i = 1;
	} else {
		i = 0;
	}

	for (;i < 2; i++) {

		while(local_table != NULL) {
			if(strcmp(local_table->id, symbol_id) == 0) {
				return local_table;
			}
			if(i == 0) {
				local_table = local_table->children;
			} else {
				local_table = local_table->next;
			}
		}

		local_table = root_st;
	}

	return NULL;
}

char* search_func_type(char* func_id) { /* NOT USED */
	symbol_node* global_table = root_st;

	while(global_table != NULL) {
		if(strcmp(global_table->id, func_id) == 0) {
			while(strcmp(global_table->id, "return") != 0) {
				global_table = global_table->children;
			}
			return global_table->type;
		}

		global_table = global_table->next;
	}

	return strdup("undef");
}

/* CHANGED THIS */
char* check_store_op(node_type* node) {
	char first_annotation[1024];
	char second_annotation[1024];

	strcpy(first_annotation, (node->children)->annotation);
	strcpy(second_annotation, ((node->children)->siblings)->annotation);

	if(strcmp(first_annotation, "undef") == 0 || strchr(first_annotation, '[') != NULL || strchr(first_annotation, '(') != NULL) {
		printf("Line %d, col %d: Operator = cannot be applied to types %s, %s\n", node->token_line, node->token_col, first_annotation, second_annotation);
		return strdup(first_annotation);
	}

	if(strchr(second_annotation, '[') != NULL) {
		change_array_to_pointer(second_annotation);
	}

	/* checks if the first operand is a lvalue */
	if(strcmp((node->children)->type, "Id") == 0 || strcmp((node->children)->type, "Deref") == 0) {
		
		/* checks if the first operand has integral type */
		if(strcmp(first_annotation, "int") == 0 || strcmp(first_annotation, "char") == 0) {

			/* checks if the second operand has integral type */
			if(strcmp(second_annotation, "int") == 0 || strcmp(second_annotation, "char") == 0) {
				return strdup(first_annotation);
			}
			
		} else if(is_pointer_to_object(first_annotation)) { /* checks if the first operand is a pointer to an object */

			/* checks if the second operand is also a pointer to an object */
			if(is_pointer_to_object(second_annotation)) {

				/* checks if the operands are pointer to objects of the same type */
				if(strcmp(first_annotation, second_annotation) == 0) {
					return strdup(first_annotation);
				}

			} else if(strcmp(second_annotation, "void*") == 0) { /* checks if the second operand is a pointer to void */
				return strdup(first_annotation);
			}

		} else if(strcmp(first_annotation, "void*") == 0) { /* checks if the first operand is a pointer to void */

			/* checks if the second operand is a pointer to an object or a pointer to void */
			if(is_pointer(second_annotation)) {
				return strdup(first_annotation);
			}
		}

		/* checks if the first operand is a pointer to an object or a pointer to void */
		if(is_pointer(first_annotation)) {

			/* checks if the second operand is a constant expression with value zero */
			if(strcmp(((node->children)->siblings)->type, "IntLit") == 0 && strcmp(((node->children)->siblings)->token, "0") == 0) {
				return strdup(first_annotation);
			}
		}
	} else {
		printf("Line %d, col %d: Lvalue required\n", (node->children)->token_line, (node->children)->token_col);
		return strdup(first_annotation);
	}

	printf("Line %d, col %d: Operator = cannot be applied to types %s, %s\n", node->token_line, node->token_col, first_annotation, ((node->children)->siblings)->annotation);
	return strdup(first_annotation);
}

/* CHANGED THIS */
char* check_not_op(node_type* node) {

	char operand_type[1024];

	strcpy(operand_type, (node->children)->annotation);

	if(strchr(operand_type, '[') != NULL) {
		change_array_to_pointer(operand_type);
	}

	/* checks if the operand has integral type or if it is a pointer */
	if(strcmp(operand_type, "int") == 0 || strcmp(operand_type, "char") == 0 || is_pointer(operand_type)) {
		return strdup("int");

	} else {
		printf("Line %d, col %d: Operator ! cannot be applied to type %s\n", node->token_line, node->token_col, (node->children)->annotation);
		return strdup("int");

	}	
}

char* check_relational_op(node_type* node) {

	char first_annotation[1024];
	char second_annotation[1024];

	strcpy(first_annotation, (node->children)->annotation);
	strcpy(second_annotation, ((node->children)->siblings)->annotation);

	if(strchr(first_annotation, '[') != NULL) {
		change_array_to_pointer(first_annotation);
	}

	if(strchr(second_annotation, '[') != NULL) {
		change_array_to_pointer(second_annotation);
	}

	/* checks if the first operand has integral type */
	if(strcmp(first_annotation, "int") == 0 || strcmp(first_annotation, "char") == 0) {

		/* checks if the second operand also has integral type */
		if(strcmp(second_annotation, "int") == 0 || strcmp(second_annotation, "char") == 0) {
			return strdup("int");
		}

	} else if(is_pointer_to_object(first_annotation)) { /* checks if the first operand is a pointer to an object */

		/* checks if the second operand is also a pointer */
		if(is_pointer_to_object(second_annotation)) {
			
			/* checks if the operands are pointers to objects of the same type */
			if(strcmp(first_annotation, second_annotation) == 0) {
				return strdup("int");
			}
		}
	}

	/* otherwise the operator cannot be applied to the operands */
	if(strcmp(node->type, "Lt") == 0) {
		printf("Line %d, col %d: Operator < cannot be applied to types %s, %s\n", node->token_line, node->token_col, (node->children)->annotation, ((node->children)->siblings)->annotation);
	} else if(strcmp(node->type, "Gt") == 0) {
		printf("Line %d, col %d: Operator > cannot be applied to types %s, %s\n", node->token_line, node->token_col, (node->children)->annotation, ((node->children)->siblings)->annotation);
	} else if(strcmp(node->type, "Le") == 0) {
		printf("Line %d, col %d: Operator <= cannot be applied to types %s, %s\n", node->token_line, node->token_col, (node->children)->annotation, ((node->children)->siblings)->annotation);
	} else if(strcmp(node->type, "Ge") == 0) {
		printf("Line %d, col %d: Operator >= cannot be applied to types %s, %s\n", node->token_line, node->token_col, (node->children)->annotation, ((node->children)->siblings)->annotation);
	}
	
	return strdup("int");
}

char* check_equality_op(node_type* node) {
	
	/* NOTE: The rules are slightly different from those of the relational operators */

	int is_valid = 0;

	char first_annotation[1024];
	char second_annotation[1024];

	strcpy(first_annotation, (node->children)->annotation);
	strcpy(second_annotation, ((node->children)->siblings)->annotation);

	if(strchr(first_annotation, '[') != NULL) {
		change_array_to_pointer(first_annotation);
	}

	if(strchr(second_annotation, '[') != NULL) {
		change_array_to_pointer(second_annotation);
	}

	/* checks if the first operand has integral type */
	if(strcmp(first_annotation, "int") == 0 || strcmp(first_annotation, "char") == 0) {

		/* checks if the second operand also has integral type */
		if(strcmp(second_annotation, "int") == 0 || strcmp(second_annotation, "char") == 0) {
			is_valid = 1;
		}

	} else if(is_pointer_to_object(first_annotation)) { /* checks if the first operand is a pointer to an object */

		/* checks if the second operand is also a pointer to an object */
		if(is_pointer_to_object(second_annotation)) {
			
			/* checks if the operands are pointers to objects of the same type */
			if(strcmp(first_annotation, second_annotation) == 0) {
				is_valid = 1;
			}

		} else if(strcmp(second_annotation, "void*") == 0) { /* checks if the second operand is a pointer to void */
			is_valid = 1;
		}

	} else if(strcmp(first_annotation, "void*") == 0) { /* cheks if the first operand is a pointer to void */
		
		/* checks if the second operand is a pointer to an object or if it is also a pointer to void */
		if(is_pointer(second_annotation)) {
			is_valid = 1;
		}
	}

	/* checks if the first operand is a constant integral expression with value zero */
	if(strcmp((node->children)->type, "IntLit") == 0 && strcmp((node->children)->token, "0") == 0) {

		/* checks if the second operand is a pointer to an object or a pointer to void */
		if(is_pointer(second_annotation)) {
			is_valid = 1;
		}

	} else if(is_pointer(first_annotation)) { /* checks if the first operand is a pointer to an object or a pointer to void */
		
		/* checks if the second operand is a constant integral expression with value zero */
		if(strcmp(((node->children)->siblings)->type, "IntLit") == 0 && strcmp(((node->children)->siblings)->token, "0") == 0) {
			is_valid = 1;
		}
	}

	if(is_valid) {

		return strdup("int");
		
	} else {

		if(strcmp(node->type, "Eq") == 0) {
			printf("Line %d, col %d: Operator == cannot be applied to types %s, %s\n", node->token_line, node->token_col, (node->children)->annotation, ((node->children)->siblings)->annotation);
		} else {
			printf("Line %d, col %d: Operator != cannot be applied to types %s, %s\n", node->token_line, node->token_col, (node->children)->annotation, ((node->children)->siblings)->annotation);
		}

		return strdup("int");
	}
}

char* check_logical_op(node_type* node) {
	/* TODO: should a array be considered a pointer or not? If so, should it be converted to a pointer when the error is printed? */

	int is_valid = 0;

	char first_annotation[1024];
	char second_annotation[1024];

	strcpy(first_annotation, (node->children)->annotation);
	strcpy(second_annotation, ((node->children)->siblings)->annotation);

	if(strchr(first_annotation, '[') != NULL) {
		change_array_to_pointer(first_annotation);
	}

	if(strchr(second_annotation, '[') != NULL) {
		change_array_to_pointer(second_annotation);
	}

	/* checks if the first operand has integral type or if it is a pointer */
	if(strcmp(first_annotation, "int") == 0 || strcmp(first_annotation, "char") == 0 || is_pointer(first_annotation)) {

		/* checks if the second operand has integral type or if it is a pointer */
		if(strcmp(second_annotation, "int") == 0 || strcmp(second_annotation, "char") == 0 || is_pointer(second_annotation)) {
			is_valid = 1;
		}
	}

	if(is_valid) {
		return strdup("int");

	} else {

		if(strcmp(node->type, "And") == 0)
			printf("Line %d, col %d: Operator && cannot be applied to types %s, %s\n", node->token_line, node->token_col, (node->children)->annotation, ((node->children)->siblings)->annotation);
		else
			printf("Line %d, col %d: Operator || cannot be applied to types %s, %s\n", node->token_line, node->token_col, (node->children)->annotation, ((node->children)->siblings)->annotation);

		return strdup("int");
	}
}

char* check_multiplicative_op(node_type* node) {

	char first_annotation[1024];
	char second_annotation[1024];

	strcpy(first_annotation, (node->children)->annotation);
	strcpy(second_annotation, ((node->children)->siblings)->annotation);

	/* checks whether the first operand is valid or not */
	if(strcmp(first_annotation, "int") == 0 || strcmp(first_annotation, "char") == 0) {
		
		/* must check if the second operand is also valid */
		if(strcmp(second_annotation, "int") == 0 || strcmp(second_annotation, "char") == 0) {
			return strdup("int");
		} else if(strchr(second_annotation, '[') != NULL) {
			change_array_to_pointer(second_annotation);
		}

	} else if(strchr(first_annotation, '[') != NULL) {
		change_array_to_pointer(first_annotation);
	}

	if(strcmp(node->type, "Mul") == 0) {
		printf("Line %d, col %d: Operator * cannot be applied to types %s, %s\n", node->token_line, node->token_col, (node->children)->annotation, ((node->children)->siblings)->annotation);
	} else if (strcmp(node->type, "Div") == 0) {
		printf("Line %d, col %d: Operator / cannot be applied to types %s, %s\n", node->token_line, node->token_col, (node->children)->annotation, ((node->children)->siblings)->annotation);
	} else {
		printf("Line %d, col %d: Operator %% cannot be applied to types %s, %s\n", node->token_line, node->token_col, (node->children)->annotation, ((node->children)->siblings)->annotation);
	}

	return strdup("int");
}

/* CHANGED THIS */
char* check_comma_op(node_type* node) {
	/* TODO: right now, the array is converted to a pointer */
	char first_annotation[1024];
	char second_annotation[1024];
	
	strcpy(first_annotation, (node->children)->annotation);
	strcpy(second_annotation, ((node->children)->siblings)->annotation);
	
	if(strchr(first_annotation, '(') != NULL) {
		sprintf(first_annotation, "undef");
	}

	if(strchr(second_annotation, '(') != NULL) {
		sprintf(second_annotation, "undef");
	}
		
	if(strcmp(second_annotation, "undef") == 0) {
		printf("Line %d, col %d: Operator , cannot be applied to types %s, %s\n", node->token_line, node->token_col, (node->children)->annotation, ((node->children)->siblings)->annotation);
		return strdup("undef");
	} else if (strcmp(first_annotation, "undef") == 0) {
		printf("Line %d, col %d: Operator , cannot be applied to types %s, %s\n", node->token_line, node->token_col, (node->children)->annotation, ((node->children)->siblings)->annotation);
	}

	if(strchr(second_annotation, '[') != NULL) {
		change_array_to_pointer(second_annotation);
	}

	return strdup(second_annotation);
}

char* check_unary_op(node_type* node) {
	/* I believe no further verifications need to be performed */
	/* TODO: should an array be converted to pointer when an error occurs? */
	
	if(!(strcmp((node->children)->annotation, "int") == 0 || strcmp((node->children)->annotation, "char") == 0)) {
		if(strcmp(node->type, "Plus") == 0) {
			printf("Line %d, col %d: Operator + cannot be applied to type %s\n", node->token_line, node->token_col, (node->children)->annotation);
		} else {
			printf("Line %d, col %d: Operator - cannot be applied to type %s\n", node->token_line, node->token_col, (node->children)->annotation);
		}
	}

	return strdup("int");
}

char* check_add_op(node_type* node, int flag_deref) {

	node_type* first_operand = node->children;
	node_type* second_operand = (node->children)->siblings;
	
	/* variables used when the annotations must suffer some change in order to correctly represent pointers */
	char first_annotation[1024];
	char second_annotation[1024];

	strcpy(first_annotation, first_operand->annotation);
	strcpy(second_annotation, second_operand->annotation);

	if(strchr(first_annotation, '[') != NULL) {
		change_array_to_pointer(first_annotation);
	}

	if(strchr(second_annotation, '[') != NULL) {
		change_array_to_pointer(second_annotation);
	}

	/* checks if the first operand has integral type */
	if(strcmp(first_annotation, "int") == 0 || strcmp(first_annotation, "char") == 0) {
		
		/* checks if the second operand has integral type */
		if(strcmp(second_annotation, "int") == 0 || strcmp(second_annotation, "char") == 0) {
			return strdup("int");

		} else if(is_pointer(second_annotation)) { /* checks if the first operand is a pointer to an object or a pointer to void */
			return strdup(second_annotation);

		}
	
	} else if(is_pointer(first_annotation)) { /* checks if the first operand is a pointer to an object or a pointer to void */

		/* checks if the second operand has integral type */
		if(strcmp(second_annotation, "int") == 0 || strcmp(second_annotation, "char") == 0) {
			return strdup(first_annotation);
		}

	}

	/* otherwise the operator cannot be applied to the types of the operands */
	if(flag_deref != 1) {
		printf("Line %d, col %d: Operator + cannot be applied to types %s, %s\n", node->token_line, node->token_col, first_operand->annotation, second_operand->annotation);
	}

	return strdup("undef");
}

char* check_sub_op(node_type* node) {
	
	node_type* first_operand = node->children;
	node_type* second_operand = (node->children)->siblings;

	/* variables used when the annotations must suffer some change in order to correctly represent pointers */
	char first_annotation[1024];
	char second_annotation[1024];

	strcpy(first_annotation, first_operand->annotation);
	strcpy(second_annotation, second_operand->annotation);

	if(strchr(first_annotation, '[') != NULL) {
		change_array_to_pointer(first_annotation);
	}

	if(strchr(second_annotation, '[') != NULL) {
		change_array_to_pointer(second_annotation);
	}

	/* checks if the first operand has integral type */
	if(strcmp(first_annotation, "int") == 0 || strcmp(first_annotation, "char") == 0) {

		/* checks if the second operand has integral type */
		if(strcmp(second_annotation, "int") == 0 || strcmp(second_annotation, "char") == 0) {
			return strdup("int");
		}

	} else if(is_pointer(first_annotation)) { /* checks if the first operand is a pointer to an object or a pointer to void */
		
		/* checks if the second operand has integral type */
		if(strcmp(second_annotation, "int") == 0 || strcmp(second_annotation, "char") == 0) {
			return strdup(first_annotation);

		} else if(is_pointer(second_annotation)) { /* checks if the second operand is a pointer to an object or a pointer to void */
			
			/* checks if the operands are pointers to objects of the same type */
			if(strcmp(first_annotation, second_annotation) == 0) {
				return strdup("int");
				
			}
		}
	}

	/* otherwise the operator cannot be applied to the types of the operands */
	printf("Line %d, col %d: Operator - cannot be applied to types %s, %s\n", node->token_line, node->token_col, first_operand->annotation, second_operand->annotation);
	return strdup("undef");
}

char* check_deref_op(node_type* node) {
	/* TODO: can you really do something like *(void*)? */

	char operand_type[1024];

	strcpy(operand_type, (node->children)->annotation);

	if(strchr(operand_type, '[') != NULL) {
		change_array_to_pointer(operand_type);
	}

	/* checks if the operand is a pointer */
	if(is_pointer(operand_type)) {
		operand_type[strlen(operand_type)-1] = '\0';
		return strdup(operand_type);

	} else { /* the operand isn't a pointer and the operator cannot be applied */
		printf("Line %d, col %d: Operator * cannot be applied to type %s\n", node->token_line, node->token_col, operand_type);
		return strdup("undef");
	}
}

char* check_deref_array_op(node_type* node) {
	/* TODO: can you really do something like *(void*)? */
	char operand_type[1024];

	(node->children)->annotation = check_add_op(node->children, 1);
	
	strcpy(operand_type, (node->children)->annotation);

	if (strcmp(operand_type, "undef") == 0) {
		printf("Line %d, col %d: Operator [ cannot be applied to types %s, %s\n", node->token_line, node->token_col, ((node->children)->children)->annotation, (((node->children)->children)->siblings)->annotation);
		return strdup("undef");
	}
	
	if(strchr(operand_type, '[') != NULL) {
		change_array_to_pointer(operand_type);
	}

	/* checks if the operand is a pointer */
	if(is_pointer(operand_type)) {
		operand_type[strlen(operand_type)-1] = '\0';
		return strdup(operand_type);

	} else { /* the operand isn't a pointer and the operator cannot be applied */
		printf("Line %d, col %d: Operator [ cannot be applied to types %s, %s\n", node->token_line, node->token_col, ((node->children)->children)->annotation, (((node->children)->children)->siblings)->annotation);
		return strdup("undef");
	}
}

/* CHANGED THIS */
char* check_addr_op(node_type* node) {
	/*
	** TODO: can you do something like &(*(void*)) or &void?
	**
	** TODO: must check if the type of error is correct.
	*/

	char operand_type[1024];

	strcpy(operand_type, (node->children)->annotation);

	/* checks if the operand has an undefined type or if it is the ID of a function */
	if(strcmp(operand_type, "undef") == 0 || strchr(operand_type, '(') != NULL) {
		printf("Line %d, col %d: Operator & cannot be applied to type %s\n", node->token_line, node->token_col, operand_type);
		return strdup("undef");

	} else if(strcmp(operand_type, "void") == 0) { /* checks if the operand has void type */
		printf("Line %d, col %d: Lvalue required\n", (node->children)->token_line, (node->children)->token_col);
		return strdup("undef");
	}

	if(strchr(operand_type, '[') != NULL) {
		/* change_array_to_pointer(operand_type); */
		printf("Line %d, col %d: Operator & cannot be applied to type %s\n", node->token_line, node->token_col, operand_type);
		return strdup("undef");
	}

	if(!(strcmp((node->children)->type, "Id") == 0 || strcmp((node->children)->type, "Deref") == 0)) {
		printf("Line %d, col %d: Lvalue required\n", (node->children)->token_line, (node->children)->token_col);
	}

	memset(operand_type+strlen(operand_type)+1, 0, 1);
	operand_type[strlen(operand_type)] = '*';
	return strdup(operand_type);
}

void check_if(node_type* node) {
	char condition_annotation[1024];

	strcpy(condition_annotation, (node->children)->annotation);

	if(strchr(condition_annotation, '[') != NULL) {
		change_array_to_pointer(condition_annotation);
	}

	if(!(strcmp(condition_annotation, "int") == 0 || strcmp(condition_annotation, "char") == 0 || is_pointer(condition_annotation))) {
		printf("Line %d, col %d: Conflicting types (got %s, expected int)\n", (node->children)->token_line, (node->children)->token_col, (node->children)->annotation);
	}
}

void check_for(node_type* node) {
	char condition_annotation[1024];

	if(strcmp(((node->children)->siblings)->type, "Null") == 0) {
		return;
	}

	strcpy(condition_annotation, ((node->children)->siblings)->annotation);

	if(strchr(condition_annotation, '[') != NULL) {
		change_array_to_pointer(condition_annotation);
	}

	if(!(strcmp(condition_annotation, "int") == 0 || strcmp(condition_annotation, "char") == 0 || is_pointer(condition_annotation))) {
		printf("Line %d, col %d: Conflicting types (got %s, expected int)\n", ((node->children)->siblings)->token_line, ((node->children)->siblings)->token_col, ((node->children)->siblings)->annotation);
	}
}

int is_pointer_to_object(char* type_to_check) {
	/* checks if the annotation passed as a parameter corresponds to a pointer and, if so, if it isn't a pointer to void */

	int is_valid = 0;

	if(strchr(type_to_check, '(') == NULL) {

		if(strchr(type_to_check, '*') != NULL && strcmp(type_to_check, "void*") != 0)
			is_valid = 1;
	}

	return is_valid;
}

int is_pointer(char* type_to_check) {
	/* checks if the annotation passed as a parameter corresponds to a pointer to an object or to a pointer to void */

	int is_valid = 0;

	if(strchr(type_to_check, '(') == NULL && strchr(type_to_check, '*') != NULL) {
		is_valid = 1;
	}

	return is_valid;
}

int check_func_errors(symbol_node* old, symbol_node* new, int line, int col) {
	
	if(strcmp(old->type, new->type) != 0) {
		/* NOT EQUAL PARAMETERS OR TYPE */
		printf("Line %d, col %d: Conflicting types (got %s, expected %s)\n", line, col, new->type, old->type);
	} else if (new->flag.flag_param == 1) {
		return 1;
	}

	return 0;
}

/* CHANGED THIS */
char* check_call(node_type* node, symbol_node* table) {
	node_type* id_call = node->children;
	char* return_type;
	int func_params, node_params = 0, find_error = 0;
	node_type* params;
	
	table = search_symbol_table(id_call->token, table);

	if(table != NULL) {
		if(table->tipo == Func) {
			node = id_call->siblings; /* NODE APONTA PARA O 1º PARAMETRO */
			return_type = (table->children)->type; /* TIPO DO RETURN DA FUNÇÃO */
			func_params = n_params_on_func(table); /* NÚMERO DE PARAMETROS DA FUNÇÃO */
			table = (table->children)->children; /* TABLE APONTA PARA DEPOIS DE RETURN */

			params = node;
			while (params != NULL) {
				node_params++;
				params = params->siblings;
			}

			if(func_params == node_params) {
				while (node != NULL && table != NULL && table->flag.flag_param) {
					if(check_compatibility(node, table->type) < 0) {
						printf("Line %d, col %d: Conflicting types (got %s, expected %s)\n", node->token_line, node->token_col, node->annotation, table->type);
					}
					node = node->siblings;
					table = table->children;
				}
			} else {
				printf("Line %d, col %d: Wrong number of arguments to function %s (got %d, required %d)\n", id_call->token_line, id_call->token_col, id_call->token, node_params, func_params);
			}	
		} else {
			printf("Line %d, col %d: Symbol %s is not a function\n", id_call->token_line, id_call->token_col, id_call->token);
			find_error = 1;
		}
	} else {
		/*CHECK_ID GENERATES THE ERROR */
		/*printf("Line %d, col %d: Unknown symbol %s\n", id_call->token_line, id_call->token_col, id_call->token);*/
		find_error = 1;
	}

	if (find_error == 0) {
		return strdup(return_type);		
	}

	return strdup("undef");
}

void check_return(node_type* ret, symbol_node* table) {
	if(strcmp((table->children)->type, "void") == 0) {
		if(strcmp((ret->children)->type, "Null") == 0) {
			return;
		}
	}

	if(strcmp((ret->children)->type, "Null") != 0) {
	/*
		strcpy(return_buf, (ret->children)->annotation);

		if((ret->children)->annotation != NULL && strchr((ret->children)->annotation, '[') != NULL) {
			change_array_to_pointer(return_buf);
		}
	*/
		if(check_compatibility(ret->children, (table->children)->type) > 0) {
			return;
		}
		
		printf("Line %d, col %d: Conflicting types (got %s, expected %s)\n", (ret->children)->token_line, (ret->children)->token_col, (ret->children)->annotation, (table->children)->type);
		return;
	}
	
	printf("Line %d, col %d: Conflicting types (got void, expected %s)\n", (ret->children)->token_line, (ret->children)->token_col, (table->children)->type);
}

int n_params_on_func(symbol_node* table) {
	int n_params = 0;
	table = (table->children)->children; /* APONTA PARA DEPOIS DE RETURN */

	while(table!=NULL && table->flag.flag_param == 1) {
		if(strcmp(table->type, "void") != 0) {
			n_params++;
		}
		table = table->children;
	}

	return n_params;
}

int check_compatibility(node_type* received_node, char* expected) { /* 1 - If compatible | -1 - If not compatible */
	char received[1024];

	strcpy(received, received_node->annotation);

	if(strchr(received, '[') != NULL) {
		change_array_to_pointer(received);
	}

	if(strcmp(received, "undef") == 0) {
		return -1;
	}
	else if (strcmp(received, expected) != 0) {
		if((strcmp(received, "int") == 0 && strcmp(expected, "char") == 0) || (strcmp(received, "char") == 0 && strcmp(expected, "int") == 0)) {
			return 1;
		} else if (strcmp(expected, "void*") == 0 && is_pointer(received)) {
			return 1;
		} else if (is_pointer(expected) && (strcmp(received, "void*") == 0 || (strcmp(received_node->type, "IntLit") == 0 && strcmp(received_node->token, "0") == 0))) {
			return 1;
		}
		return -1;
	}

	return 1;
}

int strlit_length(char *str) {
	int i, j, length = 0;
	for (i = 1; i < strlen(str)-1; i++) {
		if(str[i] == '\\') {
			if(str[i+1] == 't' || str[i+1] == 'n' || str[i+1] == '\\' || str[i+1] == '\'' || str[i+1] == '\"') {
				i = i + 1;
				length++;
			} else {
				j = 1;
				while (j < 4 && isdigit(str[i+j]) && (str[i+j] - '0') > -1 && (str[i+j] - '0') < 8) {
					j = j + 1;
				}
				
				i = i + (j - 1);

				length++;
			}
		} else {
			length++;
		}
	}

	return length + 1;
}

int num_length(int num) {
	char buffer[1024];
	
	sprintf(buffer, "%d", num);

	return strlen(buffer);
}

void change_array_to_pointer(char* annotation_to_change) {
	/* generates pointers from arrays for the annotations of the AST, based on the annotation passed as a paramter */

	int i;

	for(i=0; i<strlen(annotation_to_change); i++) {
		if(annotation_to_change[i] == '[') {
			annotation_to_change[i] = '*';
			annotation_to_change[i+1] = '\0';
		}
	}
}

/* NOT USED */
int search_l_value(char* id, symbol_node* table) {
	int i;

	if(table->tipo == Global) {
		i = 1;
	} else {
		i = 0;
	}

	for (; i < 2; i++) {

		while(table != NULL) {
			if(strcmp(table->id, id) == 0) {
				if(table->tipo == Var) {
					return 1;
				} else {
					return 0;
				}
			}
			if(i == 0) {
				table = table->children;
			} else {
				table = table->next;
			}
		}

		table = root_st;
	}

	return 0;
}


/****************************************
* FUNCTIONS USED TO FREE ALL THE MEMORY *
*****************************************/

void clear_symbol_table(symbol_node* current_node) {
	if(current_node == NULL)
		return;

	if(current_node->id != NULL) {
		free(current_node->id);
		current_node->id = NULL;
	}

	if(current_node->type != NULL) {
		free(current_node->type);
		current_node->type = NULL;
	}

	clear_symbol_table(current_node->children);
	current_node->children = NULL;
	clear_symbol_table(current_node->next);
	current_node->next = NULL;

	free(current_node);
	current_node = NULL;
}

void clear_func_table(symbol_node* current_node) {
	if(current_node == NULL)
		return;

	if(current_node->id != NULL) {
		free(current_node->id);
		current_node->id = NULL;
	}

	if(current_node->type != NULL) {
		free(current_node->type);
		current_node->type = NULL;
	}

	clear_func_table(current_node->children);
	current_node->children = NULL;

	current_node->next = NULL;
	
	free(current_node);
	current_node = NULL;
}

void clear_root_st() {
	if(root_st != NULL) {
		free(root_st);
		root_st = NULL;
	}
}