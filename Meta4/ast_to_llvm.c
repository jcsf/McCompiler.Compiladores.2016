#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ast.h"
#include "simbtab.h"
#include "ast_to_llvm.h"

int count_strlits = 0;
int count_variables = 1;
int count_labels = 1;

void ast_to_llvm(node_type* ast, symbol_node* table) {
	char local;
	int arg;
	char load_var[1024];
	
	if(ast != NULL) {
		if(strcmp(ast->type,"Program") == 0) {
			/* FIND STRLITS */
			get_strlit_to_llvm(ast->children);
			
			/* DECLARE ATOI */
			printf("declare i32 @atoi(i8* nocapture) nounwind readonly\n");

			/* DEFINE ITOA */
			printf("@.stritoa = private unnamed_addr constant [3 x i8] c\"%%d\\00\"\n");
			printf("define i8* @itoa(i32 %%n, i8* %%buf) {\n");
			printf("\t%%1 = alloca i32\n");
			printf("\t%%2 = alloca i8*\n");
			printf("\tstore i32 %%n, i32* %%1\n");
			printf("\tstore i8* %%buf, i8** %%2\n");
			printf("\t%%3 = load i8** %%2\n");
			printf("\t%%4 = load i32* %%1\n");
			printf("\t%%5 = call i32 (i8*, i8*, ...)* @sprintf(i8* %%3, i8* getelementptr inbounds ([3 x i8]* @.stritoa, i32 0, i32 0), i32 %%4)\n");
			printf("\t%%6 = load i8** %%2\n");
			printf("\tret i8* %%6\n");
			printf("}\n");
			printf("declare i32 @sprintf(i8*, i8*, ...)\n");
			
			/* DECLARE PUTS */
			printf("declare i32 @puts(i8* nocapture) nounwind\n");

			ast_to_llvm(ast->children, table);
			
			return;
		
		} else if (strcmp(ast->type,"Declaration") == 0) {
			llvm_declaration(ast->children, table);

		} else if(strcmp(ast->type,"ArrayDeclaration") == 0) {
			llvm_array_declaration(ast->children, table);

		} else if (strcmp(ast->type,"FuncDeclaration") == 0) {
			llvm_function_header(ast->children, table, Func_Declaration);

		} else if (strcmp(ast->type,"FuncDefinition") == 0) {
			count_variables = 1;
			llvm_function_header(ast->children, table, Func_Definition);

		} else if (strcmp(ast->type,"FuncBody") == 0) {
			ast_to_llvm(ast->children, table);
			if(strcmp((table->children)->type, "void") == 0) {
				printf("\tret void\n");
			} else if(strchr(table->children->type, '*') != NULL) {
				printf("\tret %s null\n", convert_c_type_to_llvm_type(table->children->type));
			} else {
				printf("\tret %s 0\n", convert_c_type_to_llvm_type(table->children->type));
			}
			printf("}\n");

		} else if(strcmp(ast->type,"IntLit") == 0) {
			unsigned int octal;
			
			if(ast->token[0] == '0') {
				sscanf(ast->token, "%o", &octal);
			} else {
				octal = atoi(ast->token);
			}

			/*printf("\t%%%d = alloca i32\n", count_variables);
			printf("\tstore i32 %d, i32* %%%d\n", octal, count_variables);
			printf("\t%%%d = load i32* %%%d\n", count_variables+1, count_variables);
			count_variables++;

			ast->llvm_var = malloc(512 * sizeof(char));
			sprintf(ast->llvm_var, "%%%d", count_variables);

			count_variables++;*/
			
			ast->llvm_var = malloc(512 * sizeof(char));
			sprintf(ast->llvm_var, "%d", octal);

		} else if(strcmp(ast->type,"ChrLit") == 0) {
			char* chrlit;
			chrlit = parsing_strlit(ast->token, 2);

			printf("\t%%%d = alloca i32\n", count_variables);
			printf("\tstore i32 %d, i32* %%%d\n", chrlit[0], count_variables);
			printf("\t%%%d = load i32* %%%d\n", count_variables+1, count_variables);
			count_variables++;

			ast->llvm_var = malloc(512 * sizeof(char));
			sprintf(ast->llvm_var, "%%%d", count_variables);

			count_variables++;
			
		} else if(strcmp(ast->type,"StrLit") == 0) {

			printf("\t%%%d = getelementptr inbounds [%s x i8]* %s, i32 0, i32 0\n", count_variables, get_size_from_annotation(ast->annotation), ast->llvm_var);
			sprintf(ast->llvm_var, "%%%d", count_variables);
			count_variables++;

		} else if(strcmp(ast->type,"Id") == 0) {
			
			local = search_symbol_local(ast->token, table);
			arg = number_arg(ast->token, table); 
			
			if (arg < 0) {
				sprintf(load_var, "%s", ast->token);
			} else {
				sprintf(load_var, "%d", arg);
			}
			
			if(ast->annotation != NULL && strchr(ast->annotation, '(') == NULL) { /* IF FUNCTION ID DO NOTHING */
				if(strcmp(ast->annotation, "int") == 0) {
					printf("\t%%%d = load i32* %c%s\n", count_variables, local, load_var);
					
				} else if(strcmp(ast->annotation, "char") == 0) {
					printf("\t%%%d = load i8* %c%s\n", count_variables, local, load_var);
					/*count_variables++;
					printf("\t%%%d = sext i8 %c%d to i32\n", count_variables, local, count_variables-1);*/
	
				} else if(strchr(ast->annotation, '[') != NULL) {
					printf("\t%%%d = getelementptr inbounds [%s x %s]* %c%s, i32 0, i32 0\n", count_variables, get_size_from_annotation(ast->annotation), convert_c_type_to_llvm_type(ast->annotation), local, ast->token);
				} 
				else {
					printf("\t%%%d = load %s* %c%s\n", count_variables, convert_c_type_to_llvm_type(ast->annotation), local, load_var);
				}

				ast->llvm_var = malloc(512 * sizeof(char));
				sprintf(ast->llvm_var, "%%%d", count_variables);
				count_variables++;
			}

		} else if(strcmp(ast->type,"Call") == 0) {
			/* TODO: this must be changed */
			ast_to_llvm(ast->children, table);
			llvm_call(ast, table);

		} else if(strcmp(ast->type,"Store") == 0) {
			
			if(strcmp(ast->children->type, "Deref") == 0) {

				if(strcmp(ast->children->children->type, "Add") == 0) {
					node_type* first_child = ast->children->children->children;
					node_type* second_child = ast->children->children->children->siblings;

					if(strchr(first_child->annotation, '[') != NULL) {
						ast_to_llvm(second_child, table);

						cast_llvm_types(second_child, "int");
					
						local = search_symbol_local(first_child->token, table);
					
						printf("\t%%%d = getelementptr inbounds [%s x %s]* %c%s, i32 0, i32 %s\n", count_variables, get_size_from_annotation(first_child->annotation), convert_c_type_to_llvm_type(first_child->annotation), local, first_child->token, second_child->llvm_var);
						
						ast->children->llvm_var = malloc(512 * sizeof(char));
						sprintf(ast->children->llvm_var, "%%%d", count_variables);
						
						count_variables++;

					} else if(strchr(second_child->annotation, '[') != NULL) {
						ast_to_llvm(first_child, table);

						cast_llvm_types(first_child, "int");
					
						local = search_symbol_local(second_child->token, table);
					
						printf("\t%%%d = getelementptr inbounds [%s x %s]* %c%s, i32 0, i32 %s\n", count_variables, get_size_from_annotation(second_child->annotation), convert_c_type_to_llvm_type(second_child->annotation), local, second_child->token, first_child->llvm_var);
						
						ast->children->llvm_var = malloc(512 * sizeof(char));
						sprintf(ast->children->llvm_var, "%%%d", count_variables);

						count_variables++;

					} else {
						ast_to_llvm(ast->children->children, table);

						ast->children->llvm_var = malloc(512 * sizeof(char));
						sprintf(ast->children->llvm_var, "%s", ast->children->children->llvm_var);
					}

				} else {
					ast_to_llvm(ast->children->children, table);

					ast->children->llvm_var = malloc(512 * sizeof(char));
					sprintf(ast->children->llvm_var, "%s", ast->children->children->llvm_var);
				}

				ast_to_llvm(ast->children->siblings, table);
				
			} else {
				ast_to_llvm(ast->children->siblings, table);
			}

			/* printf("DEBUG: %s %s\n", ast->children->llvm_var, ast->children->siblings->llvm_var); */

			llvm_store(ast, table);

		} else if(strcmp(ast->type, "Not") == 0) {
			ast_to_llvm(ast->children, table);
			llvm_not(ast);
			
		} else if(strcmp(ast->type, "Lt") == 0 || strcmp(ast->type, "Gt") == 0 || strcmp(ast->type, "Le") == 0 || strcmp(ast->type, "Ge") == 0) {
			ast_to_llvm(ast->children, table);
			llvm_relational(ast);

		} else if(strcmp(ast->type, "Eq") == 0 || strcmp(ast->type, "Ne") == 0) {
			ast_to_llvm(ast->children, table);
			llvm_equality(ast);
			
		} else if(strcmp(ast->type, "Or") == 0 || strcmp(ast->type, "And") == 0) {
			/*ast_to_llvm(ast->children, table);*/
			llvm_and_or(ast, table);

		} else if(strcmp(ast->type, "Mul") == 0 || strcmp(ast->type, "Div") == 0 || strcmp(ast->type, "Mod") == 0) {
			ast_to_llvm(ast->children, table);
			llvm_multiplicative(ast);

		} else if(strcmp(ast->type, "Comma") == 0) {
			ast_to_llvm(ast->children, table);
			ast->llvm_var = malloc(512 * sizeof(char));
			sprintf(ast->llvm_var, "%s", ast->children->siblings->llvm_var);
			
		} else if(strcmp(ast->type, "Plus") == 0) {
			ast_to_llvm(ast->children, table);
			ast->llvm_var = malloc(512 * sizeof(char));
			sprintf(ast->llvm_var, "%s", ast->children->llvm_var);

		} else if(strcmp(ast->type, "Minus") == 0) {
			ast_to_llvm(ast->children, table);
			printf("\t%%%d = sub i32 0, %s\n", count_variables, ast->children->llvm_var);
			ast->llvm_var = malloc(512 * sizeof(char));
			sprintf(ast->llvm_var, "%%%d", count_variables);
			count_variables++;

		} else if(strcmp(ast->type, "Add") == 0) {
			ast_to_llvm(ast->children, table);
			llvm_add(ast);
			
		} else if(strcmp(ast->type, "Sub") == 0) {
			ast_to_llvm(ast->children, table);
			llvm_sub(ast);

		} else if(strcmp(ast->type, "Deref") == 0) {
			
			if(strcmp(ast->children->type, "Add") == 0) {
				if(strchr(ast->children->children->annotation, '[') != NULL) {
					ast_to_llvm(ast->children->children->siblings, table);

					cast_llvm_types(ast->children->children->siblings, "int");
					
					local = search_symbol_local(ast->children->children->token, table);
					
					printf("\t%%%d = getelementptr inbounds [%s x %s]* %c%s, i32 0, i32 %s\n", count_variables, get_size_from_annotation(ast->children->children->annotation), convert_c_type_to_llvm_type(ast->children->children->annotation), local, ast->children->children->token, ast->children->children->siblings->llvm_var);
					count_variables++;

				} else if(strchr(ast->children->children->siblings->annotation, '[') != NULL) {
					ast_to_llvm(ast->children->children, table);

					cast_llvm_types(ast->children->children, "int");
					
					local = search_symbol_local(ast->children->children->siblings->token, table);
					
					printf("\t%%%d = getelementptr inbounds [%s x %s]* %c%s, i32 0, i32 %s\n", count_variables, get_size_from_annotation(ast->children->children->siblings->annotation), convert_c_type_to_llvm_type(ast->children->children->siblings->annotation), local, ast->children->children->siblings->token, ast->children->children->llvm_var);
					count_variables++;

				} else {
					ast_to_llvm(ast->children, table);
				}
				
			} else {
				ast_to_llvm(ast->children, table);
			}
			
			llvm_deref(ast);

		} else if(strcmp(ast->type, "Addr") == 0) {
			ast->llvm_var = malloc(512 * sizeof(char));

			if(strcmp(ast->children->type, "Id") == 0) {
				local = search_symbol_local(ast->children->token, table);
				arg = number_arg(ast->children->token, table); 
			
				if(arg < 0) {
					sprintf(load_var, "%s", ast->children->token);
				} else {
					sprintf(load_var, "%d", arg);
				}

				sprintf(ast->llvm_var, "%c%s", local, load_var);

			} else {
				if(strcmp(ast->children->children->type, "Add") == 0) {
					if(strchr(ast->children->children->children->annotation, '[') != NULL) {
						ast_to_llvm(ast->children->children->children->siblings, table);

						cast_llvm_types(ast->children->children->children->siblings, "int");
					
						local = search_symbol_local(ast->children->children->children->token, table);
					
						printf("\t%%%d = getelementptr inbounds [%s x %s]* %c%s, i32 0, i32 %s\n", count_variables, get_size_from_annotation(ast->children->children->children->annotation), convert_c_type_to_llvm_type(ast->children->children->children->annotation), local, ast->children->children->children->token, ast->children->children->children->siblings->llvm_var);
						
						sprintf(ast->llvm_var, "%%%d", count_variables);
						count_variables++;
					} else if (strchr(ast->children->children->children->siblings->annotation, '[') != NULL) {
						ast_to_llvm(ast->children->children->children, table);

						cast_llvm_types(ast->children->children->children, "int");
					
						local = search_symbol_local(ast->children->children->children->siblings->token, table);
					
						printf("\t%%%d = getelementptr inbounds [%s x %s]* %c%s, i32 0, i32 %s\n", count_variables, get_size_from_annotation(ast->children->children->children->siblings->annotation), convert_c_type_to_llvm_type(ast->children->children->children->siblings->annotation), local, ast->children->children->children->siblings->token, ast->children->children->children->llvm_var);
						
						sprintf(ast->llvm_var, "%%%d", count_variables);
						count_variables++;
					} else {
						ast_to_llvm(ast->children->children, table);
						sprintf(ast->llvm_var, "%s", ast->children->children->llvm_var);
					}

				} else {
					ast_to_llvm(ast->children->children, table);
					sprintf(ast->llvm_var, "%s", ast->children->children->llvm_var);
				}
			}
		
		} else if(strcmp(ast->type, "If") == 0) {
			llvm_if(ast, table);

		} else if(strcmp(ast->type, "For") == 0) {
			llvm_for(ast, table);

		} else if(strcmp(ast->type, "Return") == 0) {
			ast_to_llvm(ast->children, table);
			
			if(strcmp((table->children)->type, "void") != 0) {
				if(strchr(table->children->type, '*') != NULL && strcmp(ast->children->type, "IntLit") == 0) {
					printf("\tret %s null\n", convert_c_type_to_llvm_type(table->children->type));
				} else {
					cast_llvm_types(ast->children, (table->children)->type);
					printf("\tret %s %s\n",convert_c_type_to_llvm_type((table->children)->type), ast->children->llvm_var);
				}
			}
			
			count_variables++;
			
			return;
			
		} else {
			ast_to_llvm(ast->children, table);

		}

		ast_to_llvm(ast->siblings, table);
	}

	return;
}

void llvm_function_header(node_type* ast, symbol_node* table, Func_Header_Type h_type) {
	node_type *aux;
	symbol_node* func_table, *param_aux;
	char* func_type;
	int n_params;
	int is_definition = 0;

	/* finds the function ID */
	for(aux = ast; aux != NULL && strcmp(aux->type, "Id") != 0; aux = aux->siblings);
	
	func_table = search_symbol_table(aux->token, table);
	
	
	if(h_type == Func_Declaration) {
		if(func_table->flag.flag_print == 1 || strcmp(func_table->id, "atoi") == 0 || strcmp(func_table->id, "itoa") == 0 || strcmp(func_table->id, "puts") == 0) {
			return;
		}
		printf("declare ");
	} else {
		if(strcmp(func_table->id, "atoi") == 0 || strcmp(func_table->id, "itoa") == 0 || strcmp(func_table->id, "puts") == 0) {
			return;
		}
		is_definition = 1;
		printf("define ");
	}

	
	if(func_table != NULL && func_table->tipo == Func) {
		func_type = (func_table->children)->type;

		printf("%s @%s ", convert_c_type_to_llvm_type(func_type), func_table->id);

		/* finds the parameters list */
		for(aux = ast; aux != NULL && strcmp(aux->type, "ParamList") != 0; aux = aux->siblings);

		n_params = n_params_on_func(func_table);

		if(n_params == 0) {
			
			if(!is_definition) {
				printf("();\n");
			} else {
				printf("() ");
			}

		} else {
			printf("(");
			/* Print Table Params */
			for(param_aux = (func_table->children)->children; param_aux != NULL && param_aux->flag.flag_param == 1; param_aux = param_aux->children) {
				if(!isdigit(param_aux->id[0])) {
					if(n_params > 1) {
						printf("%s %%%s, ", convert_c_type_to_llvm_type(param_aux->type), param_aux->id);
						n_params--;
					} else {
						printf("%s %%%s", convert_c_type_to_llvm_type(param_aux->type), param_aux->id);
					}
				} else {
					if(n_params > 1) {
						printf("%s, ", convert_c_type_to_llvm_type(param_aux->type));
						n_params--;
					} else {
						printf("%s", convert_c_type_to_llvm_type(param_aux->type));
					}
				}
			}

			if(!is_definition) {
				printf(");\n");
			} else {
				printf(") ");
			}
		}
	}

	if(is_definition) {
		printf("{\n");
		n_params = n_params_on_func(func_table);

		if(n_params != 0) {
			for(param_aux = (func_table->children)->children; param_aux != NULL && param_aux->flag.flag_param == 1; param_aux = param_aux->children) {
				printf("\t%%%d = alloca %s\n", count_variables, convert_c_type_to_llvm_type(param_aux->type));
				printf("\tstore %s %%%s, %s* %%%d\n", convert_c_type_to_llvm_type(param_aux->type), param_aux->id, convert_c_type_to_llvm_type(param_aux->type), count_variables);
				count_variables++;
				n_params--;
			}
		}

		ast_to_llvm(ast->siblings, func_table);
	}
}

void llvm_declaration(node_type* ast, symbol_node* table) {

	node_type* variable_id;
	symbol_node* curr_table;
	char* variable_type = NULL;

	/* finds the variable ID */
	for(variable_id = ast; variable_id->siblings != NULL; variable_id = variable_id->siblings);

	if(table->tipo == Global) {
		curr_table = table->next;

		while(curr_table != NULL) {
			if(curr_table->tipo == Var) {
				if(strcmp(variable_id->token, curr_table->id) == 0) {
					variable_type = strdup(curr_table->type);
					break;
				}
			}

			curr_table = curr_table->next;
		}

		if(strchr(convert_c_type_to_llvm_type(variable_type), '*') != NULL) {
			printf("@%s = common global %s null\n", variable_id->token, convert_c_type_to_llvm_type(variable_type));
		} else {
			printf("@%s = common global %s 0\n", variable_id->token, convert_c_type_to_llvm_type(variable_type));
		}

	} else {
		curr_table = table->children;

		while(curr_table != NULL) {
			if(curr_table->tipo == Var && curr_table->flag.flag_param != 1) {
				if(strcmp(variable_id->token, curr_table->id) == 0) {
					variable_type = strdup(curr_table->type);
					break;
				}
			}

			curr_table = curr_table->children;
		}

		printf("\t%%%s = alloca %s\n", variable_id->token, convert_c_type_to_llvm_type(variable_type));
	}
}

void llvm_array_declaration(node_type* ast, symbol_node* table) {

	node_type* array_id;
	symbol_node* curr_table;
	char* array_intlit = NULL;
	char* array_type = NULL;
	int i;

	/* finds the variable ID */
	for(array_id = ast; array_id->siblings != NULL && strcmp(array_id->type, "Id") != 0; array_id = array_id->siblings);

	array_intlit = strdup(array_id->siblings->token);

	if(table->tipo == Global) {
		curr_table = table->next;

		while(curr_table != NULL) {
			if(curr_table->tipo == Var) {
				if(strcmp(array_id->token, curr_table->id) == 0) {
					array_type = strdup(curr_table->type);
					break;
				}
			}

			curr_table = curr_table->next;
		}

		for(i=0; i<strlen(array_type); i++) {
			if(array_type[i] == '[') {
				memset(array_type+i, 0, sizeof(char));
				break;
			}
		}

		printf("@%s = common global [%s x %s] zeroinitializer\n", array_id->token, array_intlit, convert_c_type_to_llvm_type(array_type));

	} else {
		curr_table = table->children;

		while(curr_table != NULL) {
			if(curr_table->tipo == Var && curr_table->flag.flag_param != 1) {
				if(strcmp(array_id->token, curr_table->id) == 0) {
					array_type = strdup(curr_table->type);
					break;
				}
			}

			curr_table = curr_table->children;
		}

		for(i=0; i<strlen(array_type); i++) {
			if(array_type[i] == '[') {
				memset(array_type+i, 0, sizeof(char));
				break;
			}
		}

		printf("\t%%%s = alloca [%s x %s]\n", array_id->token, array_intlit, convert_c_type_to_llvm_type(array_type));
	}
}

void llvm_strlit(node_type* node) {
	char* strlit_size;
	char* strlit_token;

	strlit_size = get_size_from_annotation(node->annotation);

	strlit_token = parsing_strlit(node->token, atoi(strlit_size));

	if(!count_strlits) {
		printf("@.str = private unnamed_addr constant [%s x i8] c\"", strlit_size);
		print_hex(strlit_token);
		printf("\\00\"\n");
		node->llvm_var = malloc(512 * sizeof(char));
		sprintf(node->llvm_var, "@.str");
	} else {
		printf("@.str%d = private unnamed_addr constant [%s x i8] c\"", count_strlits, strlit_size);
		print_hex(strlit_token);
		printf("\\00\"\n");
		node->llvm_var = malloc(512 * sizeof(char));
		sprintf(node->llvm_var, "@.str%d", count_strlits);
	}

	count_strlits++;
}

void llvm_add(node_type* node) {
	char annotation[1024];

	if(strcmp(node->annotation, "int") == 0) {
		cast_llvm_types(node->children, node->annotation);
		cast_llvm_types(node->children->siblings, node->annotation);

		printf("\t%%%d = add i32 %s, %s\n", count_variables, node->children->llvm_var, node->children->siblings->llvm_var);

		node->llvm_var = malloc(512 * sizeof(char));
		sprintf(node->llvm_var, "%%%d", count_variables);

		count_variables++;

	} else if(strchr(node->annotation, '*') != NULL) {
		
		if(strcmp(node->children->annotation, "int") == 0 || strcmp(node->children->annotation, "char") == 0) {
			strcpy(annotation, node->children->siblings->annotation);
			
			if(strchr(annotation, '[') != NULL) {
				change_array_to_pointer(annotation);
			}
			
			cast_llvm_types(node->children, "int");
			printf("\t%%%d = getelementptr inbounds %s %s, i32 %s\n", count_variables, convert_c_type_to_llvm_type(annotation), node->children->siblings->llvm_var, node->children->llvm_var);
		
		} else {
			strcpy(annotation, node->children->annotation);
			
			if(strchr(annotation, '[') != NULL) {
				change_array_to_pointer(annotation);
			}

			cast_llvm_types(node->children->siblings, "int");
			printf("\t%%%d = getelementptr inbounds %s %s, i32 %s\n", count_variables, convert_c_type_to_llvm_type(annotation), node->children->llvm_var, node->children->siblings->llvm_var);
		
		}

		node->llvm_var = malloc(512 * sizeof(char));
		sprintf(node->llvm_var, "%%%d", count_variables);
		
		count_variables++;
	}
}

void llvm_sub(node_type* node) {
	char annotation[1024];

	if(strcmp(node->children->annotation, "int") == 0 || strcmp(node->children->annotation, "char") == 0) {
		cast_llvm_types(node->children, node->annotation);
		cast_llvm_types(node->children->siblings, node->annotation);

		printf("\t%%%d = sub i32 %s, %s\n", count_variables, node->children->llvm_var, node->children->siblings->llvm_var);

		node->llvm_var = malloc(512 * sizeof(char));
		sprintf(node->llvm_var, "%%%d", count_variables);

		count_variables++;

	} else if(strcmp(node->children->siblings->annotation, "int") == 0 || strcmp(node->children->siblings->annotation, "char") == 0) {
		cast_llvm_types(node->children->siblings, "int");

		printf("\t%%%d = sub i32 0, %s\n", count_variables, node->children->siblings->llvm_var);
		count_variables++;

		printf("\t%%%d = getelementptr inbounds %s %s, i32 %%%d\n", count_variables, convert_c_type_to_llvm_type(node->annotation), node->children->llvm_var, count_variables-1);

		node->llvm_var = malloc(512 * sizeof(char));
		sprintf(node->llvm_var, "%%%d", count_variables);

		count_variables++;

	} else {
		strcpy(annotation, node->children->annotation);

		if(strchr(annotation, '[') != NULL) {
			change_array_to_pointer(annotation);
		}

		printf("\t%%%d = ptrtoint %s %s to i64\n", count_variables, convert_c_type_to_llvm_type(annotation), node->children->llvm_var);
		printf("\t%%%d = ptrtoint %s %s to i64\n", count_variables + 1, convert_c_type_to_llvm_type(annotation), node->children->siblings->llvm_var);

		printf("\t%%%d = sub i64 %%%d, %%%d\n", count_variables+2, count_variables, count_variables+1);

		if(strcmp(annotation, "char*") != 0) {
			if(strcmp(annotation, "int*") == 0) {
				printf("\t%%%d = sdiv exact i64 %%%d, 4\n", count_variables+3, count_variables+2);
			} else {
				printf("\t%%%d = sdiv exact i64 %%%d, 8\n", count_variables+3, count_variables+2);
			}

			printf("\t%%%d = trunc i64 %%%d to i32\n", count_variables+4, count_variables+3);
			node->llvm_var = malloc(512 * sizeof(char));
			sprintf(node->llvm_var, "%%%d", count_variables+4);
			count_variables += 5;

		} else {
			printf("\t%%%d = trunc i64 %%%d to i32\n", count_variables+3, count_variables+2);
			node->llvm_var = malloc(512 * sizeof(char));
			sprintf(node->llvm_var, "%%%d", count_variables+3);
			count_variables += 4;
		}
	}
}

void llvm_multiplicative(node_type* node) {
	cast_llvm_types(node->children, node->annotation);
	cast_llvm_types(node->children->siblings, node->annotation);

	if(strcmp(node->type, "Mul") == 0) {
		printf("\t%%%d = mul i32 %s, %s\n", count_variables, node->children->llvm_var, node->children->siblings->llvm_var);
	} else if(strcmp(node->type, "Div") == 0) {
		printf("\t%%%d = sdiv i32 %s, %s\n", count_variables, node->children->llvm_var, node->children->siblings->llvm_var);
	} else {
		printf("\t%%%d = srem i32 %s, %s\n", count_variables, node->children->llvm_var, node->children->siblings->llvm_var);
	}

	node->llvm_var = malloc(512 * sizeof(char));
	sprintf(node->llvm_var, "%%%d", count_variables);

	count_variables++;
}

void llvm_relational(node_type* node) {
	char children_annotation[1024];

	if(strcmp(node->children->annotation, "int") == 0 || strcmp(node->children->annotation, "char") == 0) {
		cast_llvm_types(node->children, node->annotation);
		cast_llvm_types(node->children->siblings, node->annotation);

		if(strcmp(node->type, "Lt") == 0) {
			printf("%%%d = icmp slt i32 %s, %s\n", count_variables, node->children->llvm_var, node->children->siblings->llvm_var);

		} else if(strcmp(node->type, "Gt") == 0) {
			printf("%%%d = icmp sgt i32 %s, %s\n", count_variables, node->children->llvm_var, node->children->siblings->llvm_var);

		} else if(strcmp(node->type, "Le") == 0) {
			printf("%%%d = icmp sle i32 %s, %s\n", count_variables, node->children->llvm_var, node->children->siblings->llvm_var);

		} else {
			printf("%%%d = icmp sge i32 %s, %s\n", count_variables, node->children->llvm_var, node->children->siblings->llvm_var);

		}
	} else {

		strcpy(children_annotation, node->children->annotation);

		if(strchr(children_annotation, '[') != NULL) {
			change_array_to_pointer(children_annotation);
		}

		if(strcmp(node->type, "Lt") == 0) {
			printf("%%%d = icmp ult %s %s, %s\n", count_variables, convert_c_type_to_llvm_type(children_annotation), node->children->llvm_var, node->children->siblings->llvm_var);

		} else if(strcmp(node->type, "Gt") == 0) {
			printf("%%%d = icmp ugt %s %s, %s\n", count_variables, convert_c_type_to_llvm_type(children_annotation), node->children->llvm_var, node->children->siblings->llvm_var);

		} else if(strcmp(node->type, "Le") == 0) {
			printf("%%%d = icmp ule %s %s, %s\n", count_variables, convert_c_type_to_llvm_type(children_annotation), node->children->llvm_var, node->children->siblings->llvm_var);

		} else {
			printf("%%%d = icmp uge %s %s, %s\n", count_variables, convert_c_type_to_llvm_type(children_annotation), node->children->llvm_var, node->children->siblings->llvm_var);

		}
	}

	printf("%%%d = zext i1 %%%d to i32\n", count_variables+1, count_variables);

	node->llvm_var = malloc(512 * sizeof(char));
	sprintf(node->llvm_var, "%%%d", count_variables+1);

	count_variables += 2;
}

void llvm_equality(node_type* node) {
	char first_annotation[1024];
	char second_annotation[1024];

	if(strcmp(node->children->annotation, "int") == 0 || strcmp(node->children->annotation, "char") == 0) {
		cast_llvm_types(node->children, node->annotation);
		cast_llvm_types(node->children->siblings, node->annotation);

		if(strcmp(node->type, "Eq") == 0) {
			printf("%%%d = icmp eq i32 %s, %s\n", count_variables, node->children->llvm_var, node->children->siblings->llvm_var);

		} else {
			printf("%%%d = icmp ne i32 %s, %s\n", count_variables, node->children->llvm_var, node->children->siblings->llvm_var);

		}
	} else {

		strcpy(first_annotation, node->children->annotation);
		strcpy(second_annotation, node->children->siblings->annotation);

		if(strchr(first_annotation, '[') != NULL) {
			change_array_to_pointer(first_annotation);
		}

		if(strchr(second_annotation, '[') != NULL) {
			change_array_to_pointer(second_annotation);
		}

		if(strcmp(first_annotation, "void*") == 0) {
			cast_llvm_types(node->children, second_annotation);

			if(strcmp(node->type, "Eq") == 0) {
				printf("%%%d = icmp eq %s %s, %s\n", count_variables, convert_c_type_to_llvm_type(second_annotation), node->children->llvm_var, node->children->siblings->llvm_var);
			} else {
				printf("%%%d = icmp ne %s %s, %s\n", count_variables, convert_c_type_to_llvm_type(second_annotation), node->children->llvm_var, node->children->siblings->llvm_var);
			}
		} else if(strcmp(second_annotation, "void*") == 0) {
			cast_llvm_types(node->children->siblings, first_annotation);

			if(strcmp(node->type, "Eq") == 0) {
				printf("%%%d = icmp eq %s %s, %s\n", count_variables, convert_c_type_to_llvm_type(first_annotation), node->children->llvm_var, node->children->siblings->llvm_var);
			} else {
				printf("%%%d = icmp ne %s %s, %s\n", count_variables, convert_c_type_to_llvm_type(first_annotation), node->children->llvm_var, node->children->siblings->llvm_var);
			}
		}
	}

	printf("%%%d = zext i1 %%%d to i32\n", count_variables+1, count_variables);

	node->llvm_var = malloc(512 * sizeof(char));
	sprintf(node->llvm_var, "%%%d", count_variables+1);

	count_variables += 2;
}

void llvm_not(node_type* node) {
	char child_annotation[1024];

	strcpy(child_annotation, node->children->annotation);

	if(strchr(child_annotation, '[') != NULL) {
		change_array_to_pointer(child_annotation);
	}

	if(strcmp(node->children->annotation, "int") == 0 || strcmp(node->children->annotation, "char") == 0) {
		printf("%%%d = icmp ne %s %s, 0\n", count_variables, convert_c_type_to_llvm_type(child_annotation), node->children->llvm_var);
	} else {
		printf("%%%d = icmp ne %s %s, null\n", count_variables, convert_c_type_to_llvm_type(child_annotation), node->children->llvm_var);
	}

	printf("%%%d = xor i1 %%%d, true\n", count_variables+1, count_variables);

	printf("%%%d = zext i1 %%%d to i32\n", count_variables+2, count_variables+1);

	node->llvm_var = malloc(512 * sizeof(char));
	sprintf(node->llvm_var, "%%%d", count_variables+2);

	count_variables += 3;
}

/* ADDED THIS */
void llvm_and_or(node_type* node, symbol_node* table) {
	int first_op = count_labels;
	int second_op = count_labels + 1;
	int finish_op = count_labels + 2;
	char children1_annotation[1024];
	char children2_annotation[1024];
	node_type* save_sibling;
	
	count_labels += 3;
	
	printf("\tbr label %%lbl%d\n", first_op);
	printf("\tlbl%d:\n", first_op);
	
	if((strcmp(node->children->annotation, "int") == 0 || strcmp(node->children->annotation, "char") == 0) && (strcmp(node->children->siblings->annotation, "int") == 0 || strcmp(node->children->siblings->annotation, "char") == 0)) {

		/* FIRST */
		save_sibling = node->children->siblings;
		node->children->siblings = NULL;
		node->children->label = first_op;
		ast_to_llvm(node->children, table);
		node->children->siblings = save_sibling;
		
		cast_llvm_types(node->children, node->annotation);
		
		printf("\t%%%d = icmp ne i32 %s, 0\n", count_variables, node->children->llvm_var);
		if (strcmp(node->type, "And") == 0) {
			printf("\tbr i1 %%%d , label %%lbl%d , label %%lbl%d\n", count_variables, second_op, finish_op);
		} else {
			printf("\tbr i1 %%%d , label %%lbl%d , label %%lbl%d\n", count_variables, finish_op, second_op);
		}
		count_variables++;
		
		/* SECOND */
		printf("\tlbl%d:\n", second_op);
		
		node->children->siblings->label = second_op;
		ast_to_llvm(node->children->siblings, table);
		
		cast_llvm_types(node->children->siblings, node->annotation);
		
		printf("\t%%%d = icmp ne i32 %s, 0\n", count_variables, node->children->siblings->llvm_var);
		printf("\tbr label %%lbl%d\n", finish_op);
		count_variables++;
		
	} else {
		
		strcpy(children1_annotation, node->children->annotation);
		strcpy(children2_annotation, node->children->siblings->annotation);

		if(strchr(children1_annotation, '[') != NULL) {
			change_array_to_pointer(children1_annotation);
		}
		
		if(strchr(children2_annotation, '[') != NULL) {
			change_array_to_pointer(children2_annotation);
		}
		
		/* FIRST */
		save_sibling = node->children->siblings;
		node->children->siblings = NULL;
		node->children->label = first_op;
		ast_to_llvm(node->children, table);
		node->children->siblings = save_sibling;
		
		if (strcmp(children1_annotation, "char") == 0 || strcmp(children1_annotation, "int") == 0) {
			cast_llvm_types(node->children, node->annotation);
			printf("\t%%%d = icmp ne i32 %s, 0\n", count_variables, node->children->llvm_var);
		} else {
			printf("\t%%%d = icmp ne %s %s, null\n", count_variables, convert_c_type_to_llvm_type(children1_annotation), node->children->llvm_var);
		}
		
		if (strcmp(node->type, "And") == 0) {
			printf("\tbr i1 %%%d , label %%lbl%d , label %%lbl%d\n", count_variables, second_op, finish_op);
		} else {
			printf("\tbr i1 %%%d , label %%lbl%d , label %%lbl%d\n", count_variables, finish_op, second_op);
		}
		
		count_variables++;
		
		/* SECOND */
		printf("\tlbl%d:\n", second_op);
		
		node->children->siblings->label = second_op;
		ast_to_llvm(node->children->siblings, table);
		
		if (strcmp(children2_annotation, "char") == 0 || strcmp(children2_annotation, "int") == 0) {
			cast_llvm_types(node->children->siblings, node->annotation);
			printf("\t%%%d = icmp ne i32 %s, 0\n", count_variables, node->children->siblings->llvm_var);
		} else {
			printf("\t%%%d = icmp ne %s %s, null\n", count_variables, convert_c_type_to_llvm_type(children2_annotation), node->children->siblings->llvm_var);
		}
		printf("\tbr label %%lbl%d\n", finish_op);
		count_variables++;
			
	}
	
	printf("\tlbl%d:\n", finish_op);
		
	if (strcmp(node->type, "And") == 0) {
		printf("\t%%%d = phi i1 [ false, %%lbl%d ], [ %%%d, %%lbl%d ]\n", count_variables, node->children->label, count_variables-1, node->children->siblings->label);
	} else {
		printf("\t%%%d = phi i1 [ true, %%lbl%d ], [ %%%d, %%lbl%d ]\n", count_variables, node->children->label, count_variables-1, node->children->siblings->label);
	}

	printf("\t%%%d = zext i1 %%%d to i32\n", count_variables+1, count_variables);

	node->label = finish_op;
	
	node->llvm_var = malloc(512 * sizeof(char));
	sprintf(node->llvm_var, "%%%d", count_variables+1);

	count_variables += 2;
}

void llvm_deref(node_type* node) {
	char child_annotation[1024];
	strcpy(child_annotation, node->children->annotation);

	if(strchr(child_annotation, '[') != NULL) {
		change_array_to_pointer(child_annotation);
	}
	
	printf("\t%%%d = load %s %%%d\n", count_variables, convert_c_type_to_llvm_type(child_annotation), count_variables-1);
	
	node->llvm_var = malloc(512 * sizeof(char));
	sprintf(node->llvm_var, "%%%d", count_variables);

	count_variables++;
}

void llvm_call(node_type* node, symbol_node* local_table) {
	node_type *call = node, *params_node;
	symbol_node* call_table;
	int params, i;
	char parameters[1024];
	
	memset(parameters, 0, 1024 * sizeof(char));
	
	node = node->children;
	call_table = search_symbol_table(node->token, local_table);
	params = n_params_on_func(call_table);

	call_table = (call_table->children)->children;
	params_node = node->siblings;
	for (i = 0; i < params; i++) {
		cast_llvm_types(params_node, call_table->type);
		if (i + 1 < params) {
			sprintf(parameters + strlen(parameters), "%s %s, ", convert_c_type_to_llvm_type(call_table->type), params_node->llvm_var);
		} else {
			sprintf(parameters + strlen(parameters), "%s %s", convert_c_type_to_llvm_type(call_table->type), params_node->llvm_var);
		}
		params_node = params_node->siblings;
		call_table = call_table->children;
	}

	if (strcmp(call->annotation, "void") == 0) {
		if (params == 0) {
			printf("\tcall %s @%s()\n", convert_c_type_to_llvm_type(call->annotation), node->token);
		} else {
			printf("\tcall %s @%s(%s)\n", convert_c_type_to_llvm_type(call->annotation), node->token, parameters);		
		}
	} else {
		if (params == 0) {
			printf("\t%%%d = call %s @%s()\n", count_variables, convert_c_type_to_llvm_type(call->annotation), node->token);
		} else {
			printf("\t%%%d = call %s @%s(%s)\n", count_variables, convert_c_type_to_llvm_type(call->annotation), node->token, parameters);		
		}

		/* Var to save call return */
		call->llvm_var = malloc(512 * sizeof(char));
		sprintf(call->llvm_var, "%%%d", count_variables);
		count_variables++;
	}	
		
}

void llvm_store(node_type* node, symbol_node* table) {
	char local;
	int arg;
	char load_var[1024];

	node->llvm_var = malloc(512 * sizeof(char));

	if(strcmp(node->children->type, "Deref") != 0) {

		local = search_symbol_local(node->children->token, table);
		arg = number_arg(node->children->token, table);
		
		if (arg < 0) {
			sprintf(load_var, "%s", node->children->token);
		} else {
			sprintf(load_var, "%d", arg);
		}

		if(is_pointer(node->children->annotation) && strcmp(node->children->siblings->type, "IntLit") == 0 && strcmp(node->children->siblings->token, "0") == 0) {
			printf("\tstore %s null, %s* %c%s\n", convert_c_type_to_llvm_type(node->annotation), convert_c_type_to_llvm_type(node->annotation), local, load_var);
			sprintf(node->llvm_var, "%s", "null");
		} else {
			cast_llvm_types(node->children->siblings, node->annotation);
			printf("\tstore %s %s, %s* %c%s\n", convert_c_type_to_llvm_type(node->annotation), node->children->siblings->llvm_var, convert_c_type_to_llvm_type(node->annotation), local, load_var);
			sprintf(node->llvm_var, "%s", node->children->siblings->llvm_var);
		}
	} else {
		cast_llvm_types(node->children->siblings, node->annotation);
		printf("\tstore %s %s, %s* %s\n", convert_c_type_to_llvm_type(node->annotation), node->children->siblings->llvm_var, convert_c_type_to_llvm_type(node->annotation), node->children->llvm_var);
	}
}

void llvm_if(node_type* node, symbol_node* table) {
	int first_op = count_labels;
	int second_op = count_labels + 1;
	int finish_op = count_labels + 2;
	node_type* save_sibling;
	
	if (node->children->siblings->siblings != NULL) {
		count_labels += 3;
	} else {
		count_labels += 2;
		finish_op = second_op;
	}
	
	/* CONDIÇÃO */
	save_sibling = node->children->siblings;
	node->children->siblings = NULL;
	ast_to_llvm(node->children, table);
	node->children->siblings = save_sibling;
	printf("\t%%%d = trunc %s %s to i1", count_variables, convert_c_type_to_llvm_type(node->children->annotation),node->children->llvm_var);
	printf("\tbr i1 %%%d, label %%lbl%d , label %%lbl%d\n", count_variables, first_op, second_op);
	count_variables++;
	
	/* IF */
	printf("\tlbl%d:\n", first_op);
	save_sibling = node->children->siblings->siblings;
	node->children->siblings->siblings = NULL;
	ast_to_llvm(node->children->siblings, table);
	node->children->siblings->siblings = save_sibling;
	printf("\tbr label %%lbl%d\n", finish_op);
	
	
	/* ELSE */
	if (node->children->siblings->siblings != NULL) {
		printf("\tlbl%d:\n", second_op);
		ast_to_llvm(node->children->siblings->siblings, table);
		printf("\tbr label %%lbl%d\n", finish_op);
	}
	
	/* END IF */
	printf("\tlbl%d:\n", finish_op);
		
}


void llvm_for(node_type* node, symbol_node* table) {
	int condition = count_labels;
	int for_body = count_labels + 1;
	int for_end = count_labels + 2;
	node_type* save_sibling;
	
	count_labels += 3;
	
	/* FIRST CHILDREN */
	save_sibling = node->children->siblings;
	node->children->siblings = NULL;
	ast_to_llvm(node->children, table);
	node->children->siblings = save_sibling;
	printf("\tbr label %%lbl%d\n", condition);
	
	/* SECOND CHILDREN */
	printf("\tlbl%d:\n", condition);
	save_sibling = node->children->siblings->siblings;
	node->children->siblings->siblings = NULL;
	ast_to_llvm(node->children->siblings, table);
	node->children->siblings->siblings = save_sibling;
	printf("\t%%%d = trunc %s %s to i1", count_variables, convert_c_type_to_llvm_type(node->children->siblings->annotation),node->children->siblings->llvm_var);
	printf("\tbr i1 %%%d, label %%lbl%d , label %%lbl%d\n", count_variables, for_body, for_end);
	count_variables++;
	
	/* FOURTH CHILDREN (BODY FUNCTION)*/
	printf("\tlbl%d:\n", for_body);
	ast_to_llvm(node->children->siblings->siblings->siblings, table);
	
	/* THIRD CHILDREN */
	save_sibling = node->children->siblings->siblings->siblings;
	node->children->siblings->siblings->siblings = NULL;
	ast_to_llvm(node->children->siblings->siblings, table);
	node->children->siblings->siblings->siblings = save_sibling;
	printf("\tbr label %%lbl%d\n", condition);
	
	/*END FOR*/
	printf("\tlbl%d:\n", for_end);
}

char* convert_c_type_to_llvm_type(char* c_type) {
	int i;
	char buffer[1024];

	if(strstr(c_type,"int") != NULL) {
		sprintf(buffer,"i32");
	} else if (strcmp(c_type,"void") == 0) {
		return strdup("void");
	} else {
		sprintf(buffer,"i8");
	}
	
	for (i = 0; i < strlen(c_type); i++) {
		if(c_type[i] == '*')
			strcat(buffer, "*");
	}

	return strdup(buffer);
}

char* get_size_from_annotation(char* annotation) {
	int i;
	int curr_index = 0;
	char buffer[1024];

	for(i=0; i<strlen(annotation); i++) {
		if(isdigit(annotation[i])) {
			buffer[curr_index] = annotation[i];
			curr_index++;
		} else if (annotation[i] == ']') {
			buffer[curr_index] = '\0';
			break;
		}
	}

	return strdup(buffer);
}

void get_strlit_to_llvm(node_type* node) {
	if(node == NULL)
		return;
		
	if(strcmp(node->type, "StrLit") == 0) {
		llvm_strlit(node);
	}
	
	if (node->children != NULL)
		get_strlit_to_llvm(node->children);
		
	if (node->siblings != NULL)
		get_strlit_to_llvm(node->siblings);
}

char* parsing_strlit(char* str, int length) {
	int i, j = 1, k, octal;
	char new_str[1024];
	char nums[4];
	
	memset(new_str, 0, 1024);
	
	for (i = 0; i < length - 1; i++) {
		if(str[j] == '\\') {
			switch(str[j+1]) {
				case 't':
					new_str[i] = '\t';
					j = j + 1;
					break;
				case 'n':
					new_str[i] = '\n';
					j = j + 1;
					break;
				case '\\':
					new_str[i] = '\\';
					j = j + 1;
					break;
				case '\'':
					new_str[i] = '\'';
					j = j + 1;
					break;
				case '\"':
					new_str[i] = '\"';
					j = j + 1;
					break;
				default:
					k = 1;
					memset(nums,0,4);
					while (k < 4 && isdigit(str[j+k]) && (str[j+k] - '0') > -1 && (str[j+k] - '0') < 8) {
						nums[k-1] = str[j+k];
						k = k + 1;
					};
					sscanf(nums,"%o", &octal);
					j = j + (k - 1);
					new_str[i] = octal;
					break;
			}
		} else {
			new_str[i] = str[j];
		}
		j++;
	}
	
	return strdup(new_str);
}

void print_hex(const char *s)
{
  while(*s)
    printf("\\%02x", (unsigned int) *s++);
}

char search_symbol_local(char* id, symbol_node* local_table) {
	int i;

	if(local_table->tipo == Global) {
		i = 1;
	} else {
		local_table = local_table->children;
		i = 0;
	}

	for (; i < 2; i++) {

		while(local_table != NULL) {
			if(strcmp(local_table->id, id) == 0) {
				if(i || local_table->tipo == Func)
					return '@';
				else
					return '%';
			}
			if(i == 0) {
				local_table = local_table->children;
			} else {
				local_table = local_table->next;
			}
		}

		local_table = root_st;
	}

	return '@';
}

void cast_llvm_types(node_type* node, char* expected) {
	char annotation[1024];

	strcpy(annotation, node->annotation);

	if(strchr(annotation, '[') != NULL) {
		change_array_to_pointer(annotation);
	}

	if(strcmp(annotation, expected) == 0) {
		return;

	} else if(strcmp(annotation, "int") == 0) {
		printf("\t%%%d = trunc i32 %s to i8\n", count_variables, node->llvm_var);
		sprintf(node->llvm_var, "%%%d", count_variables);
		count_variables++;

	} else if(strcmp(annotation, "char") == 0) {
		printf("\t%%%d = sext i8 %s to i32\n", count_variables, node->llvm_var);
		sprintf(node->llvm_var, "%%%d", count_variables);
		count_variables++;

	} else if(strstr(annotation, "int") != NULL || strstr(expected, "int") != NULL) {
		printf("\t%%%d = bitcast %s %s to %s\n", count_variables, convert_c_type_to_llvm_type(annotation), node->llvm_var, convert_c_type_to_llvm_type(expected));
		sprintf(node->llvm_var, "%%%d", count_variables);
		count_variables++;

	}
}

int number_arg(char* var, symbol_node* local_table) {
	int i = 0;
	
	local_table = local_table->children;
	
	while(local_table != NULL) {
		if(strcmp(local_table->id, var) == 0) {
			if(local_table->flag.flag_param == 1)
				return i;
			else
				return -1;
		}
		
		local_table = local_table->children;
		i++;
	}
	
	return -1;
}