#include <stdio.h>
#include <stdlib.h>

char *itoa(int n, char* buf) {
	sprintf(buf, "%d", n);
	return buf;
}

int Fibonacci(int);
 
int main(int argc, char** argv) {
	char buf[20];
   int n, i, c;

   n = atoi(argv[1]);

   i = 0;
 
   for ( c = 1 ; c <= n ; c = c+1) {
   puts(itoa(Fibonacci(i), buf));
      i = i+1; 
   }
 
   return 0;
}
 
int Fibonacci(int n) {
   if ( n == 0 )
      return 0;
   else if ( n == 1 )
      return 1;
   else
      return ( Fibonacci(n-1) + Fibonacci(n-2) );
} 