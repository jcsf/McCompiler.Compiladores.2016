#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

/* CHECK THIS PART */
node_type* create_node(char* type, char* token, int line, int col) {
	node_type* new_node = (node_type*) malloc(sizeof(node_type));

	new_node->type = strdup(type);
	new_node->token = token;
	
	new_node->token_line = line;
	new_node->token_col = col;
	new_node->annotation = NULL;
	new_node->llvm_var = NULL;

	new_node->children = NULL;
	new_node->siblings = NULL;

	return new_node;
}

void add_child(node_type* parent, node_type* child) {
	parent->children = child;
}

void add_sibling(node_type* first, node_type* last) {
	if(first != NULL) {
		while(first->siblings != NULL)
			first = first->siblings;
		first->siblings = last;
	}
}

int have_sibling(node_type* node) {
	if(node != NULL && node->siblings != NULL)
		return 1;
	else
		return 0;
}

int is_null_node(node_type* node) {
	if(node != NULL && node->type != NULL && strcmp(node->type,"Null") == 0) {
		free(node->type);
		node->type = NULL;

		free(node);
		node = NULL;

		return 1;
	} else {
		return 0;
	}
}

/* CHECK THIS PART */
node_type* add_typespec_to_declarations(node_type* typespec, node_type* declarations_list) {
	node_type* aux = declarations_list;
	node_type* new_node_typespec = NULL;

	while(aux != NULL) {
		new_node_typespec = create_node(typespec->type, NULL, typespec->token_line, typespec->token_col);

		new_node_typespec->siblings = aux->children;

		aux->children = new_node_typespec;

		aux = aux->siblings;
	}

	return declarations_list;
}

/* CHECK THIS PART: RIGHT NOW IS PRINTING THE LINES AND THE COLUMNS AS DEBUG */
void print_ast(node_type* current_node, int nb_points) {
	int i;

	if(current_node == NULL)
		return;

	for(i=0; i<nb_points; i++)
		printf(".");

	if(current_node->annotation != NULL) {
		if(current_node->token != NULL && strcmp(current_node->type, "Deref")!=0)
			printf("%s(%s) - %s\n", current_node->type, current_node->token, current_node->annotation);
		else
			printf("%s - %s\n", current_node->type, current_node->annotation);
	} else {
		if(current_node->token != NULL) {
			//printf("LINE %d COL %d: %s(%s)\n", current_node->token_line, current_node->token_col, current_node->type, current_node->token);
			printf("%s(%s)\n", current_node->type, current_node->token);
		} else {
			//printf("LINE %d COL %d: %s\n", current_node->token_line, current_node->token_col, current_node->type);
			printf("%s\n", current_node->type);
		}
	}

	print_ast(current_node->children, nb_points+2);
	print_ast(current_node->siblings, nb_points);
}

/* CHECK THIS PART */
void clear_ast(node_type* current_node) {
	
	if(current_node == NULL)
		return;

	if(current_node->token != NULL) {
		free(current_node->token);
		current_node->token = NULL;
	}

	if(current_node->type != NULL) {
		free(current_node->type);
		current_node->type = NULL;
	}

	if(current_node->annotation != NULL) {
		free(current_node->annotation);
		current_node->annotation = NULL;
	}

	if(current_node->llvm_var != NULL) {
		free(current_node->llvm_var);
		current_node->llvm_var = NULL;
	}

	clear_ast(current_node->children);
	current_node->children = NULL;
	clear_ast(current_node->siblings);
	current_node->siblings = NULL;

	free(current_node);
	current_node = NULL;
}