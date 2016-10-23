/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
*/

void f(int i) {
	if (i < 10) {
		f(i+1);
	}
}

int main(int argc, char** argv) {
	int* b;

	/*
	puts("HELLO\n");

	atoi("IT'S ME\n");

	puts("Hey there!\n");

	puts("Hey");
	*/

	f(0);

	return 0;	
}