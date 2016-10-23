/*#include <stdio.h>

char *itoa(int n, char* buf) {
	sprintf(buf, "%d", n);
	return buf;
}*/

int main(void) {
	int a, b;
	char c;
	char buf[20];

	a = 0;
	b = 1;
	c = 1;

	/*puts(itoa(a || b, buf));
	puts(itoa(a || c, buf));
	puts(itoa(c || a, buf));
	puts(itoa(c || c, buf));

	puts(itoa(a && b, buf));
	puts(itoa(a && c, buf));
	puts(itoa(c && a, buf));
	puts(itoa(c && c, buf));

	puts(itoa(0 && (b = -1) && c, buf));
	puts(itoa(b, buf));

	/uts(itoa(!c || b && c, buf));

	puts(itoa((a=c) && (a=b), buf));

	puts(itoa(c || (a=b), buf)); */

	a && b && c;

	return 0;
}