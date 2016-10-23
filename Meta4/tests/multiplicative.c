#include <stdio.h>

char *itoa(int n, char* buf) {
	sprintf(buf, "%d", n);
	return buf;
}

int main(void) {
	int a;
	int b;
	char c;
	char d;
	int buffer[10];

	char buf[20];

	itoa(a % b, buf);
	itoa(c % d, buf);
	itoa(a % c, buf);
	itoa(d % b, buf);
	itoa(d % 2, buf);
	itoa(a % 2, buf);
	itoa('t' % 2, buf);
	itoa(a % 't', buf);
	itoa(buffer[1] % a, buf);

	return 0;
}