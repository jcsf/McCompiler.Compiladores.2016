char buffer[25];

/*
int func(float a, float c) {
	return 1;
}

int func(int a, int b) {
	return 0;
}
*/

/*int* func(void) {
	char* a;
	return 1;
}*/

int func(void) {
	return 1;
}

void void_func(int a) {
	return 1;
}

int main(int argc, char **argv) {
	int a, b;
	char c;
	int* d;
	int* g;
	void* f;
	void* m;
	int** array;
	int** an;
	char* k;
	char ola[25];
	char ole[24];

	/********************************************************
	********** TESTS FOR THE ADD AND SUB OPERATORS **********
	*********************************************************/
	
	/*
	b = a + buffer;
	c = a + b;
	b = a + c;
	a = d + c;
	c = d + a;
	a = b + d;
	a = f + b;
	b = a + f;
	f = f + f;
	f = buffer + buffer;
	f = a + cucu;
	a = ola - ole;

	a = buffer - c;
	a = c - d;
	d = f - a;
	f = d - a;
	d = f - m;
	d = array - an;
	f = d - g;
	f = c - d;
	f = array - c;
	f = buffer - buffer;
	*/
	
	/***********************************************************
	********** TESTS FOR THE DEREF AND ADDR OPERATORS **********
	************************************************************/
	/*
	a = *buffer;
	a = *buffer + 1;
	a = *(buffer+1);
	a = *(*buffer + 1);
	a = *(*buffer[4]);
	*/
	d = +(&a);
	d = &(*f);
	d = &m;
	d = &buffer;
	d = &(*buffer + 1);
	a = &(buffer[4]);
	a = &(*buffer[4]);

	&void_func(2);
	&(a+b);
	&8;

	/************************************************
	********** TESTS FOR LOGICAL OPERATORS **********
	*************************************************/

	/*
	a = a && b;
	a = a && c;
	a = a || d;
	a = d && f;
	f && m;
	a && f;
	*/

	/* INVALID */
	/*
	a = d || &(*f);
	a = &(*f) || d;
	a = &(*f) && c;
	c = cucu && c;
	c = cucu || c;
	c = c || cucu;
	*/

	/************************************************
	********** TESTS FOR RELATIONAL OPERATORS *******
	*************************************************/

	/*
	a = a < b;
	a = c > a;
	a = d >= a;
	b = g <= m;
	b = d > g;
	a = f > m;
	a = array <= an;
	b = b > array;
	c = c <= array;
	c = array > d;
	*/

	/************************************************
	********** TESTS FOR EQUALITY OPERATORS *********
	*************************************************/

	/*
	a == b;
	a == c;
	c == a;
	c == c;
	d == g;
	d == f;
	m == g;
	a != 0;
	0 != c;
	f != m;
	array == 0;
	0 != m;
	f == 0;
	0 != d;
	d == 0;
	array == f;
	m == buffer;
	buffer == k;
	(a + d) == g;
	*/

	/* INVALID */
	/*
	d == k;
	array == k;
	an == cucu;
	buffer == d;
	d == 1;
	1 == g;
	d == &(*f);
	array == (f - an);
	(array + an) == f;
	*/

	/******************************************************************
	********** TESTS FOR FUNCTIONS THAT ARE USED WITHOUT A CALL *******
	*******************************************************************/

	/*
	a + func;
	&8;
	&func;
	func + b;
	func - b;
	func * a;
	func / b;
	func % b;
	b - func;
	+func;
	-func;
	func == a;
	func != b;
	a <= func;
	func > a;
	*func;
	a && func;
	func && a;
	func && func;
	a,func;
	a = func;
	func = a;
	*/

	/*********************************************
	********** TESTS FOR IF/FOR STATEMENTS *******
	**********************************************/
	
	/*
	if(buffer) {
		a = 1;
	} else {
		a = 0;
	}

	if(a > 0) {
		b = 2;
	}

	if(c) {
		a = b;
	}

	if(d = g) {
		b = a;
	}

	if(&(*f)) {
		&a;
	}

	if(1) {
		a = 5;
	}
	

	for(a=0; a<b; a=a+1) {
		a = 1;
	}

	for(; ;) {

	}

	for(a=0; ; a=a+1) {
		a = 1;
	}

	for(a=0; buffer; a>b) {
		b = a;
	}

	for(b=a; 8; b>a) {
		b = 0;
	}

	for(b=a; c; b>a) {
		b = 0;
	}

	for(b=a; k; b>a) {
		b = 0;
	}

	for(b=a; void_func(1); b>a) {
		b = 0;
	}

	for(b=a; *f; b>a) {
		b = 0;
	}

	for(b=a; &(*f); b>a) {
		b = 0;
	}*/

	/********************************
	********** OTHER TESTS **********
	*********************************/

	/*
	a = atoi(argv[1]);
	b = atoi(argv[2]);
	if (a == 0) {
		puts(itoa(b, buffer));
	} else {
		for(a=1,b=2 ; b > 0 && a < b; )
			if (a > b)
				a = a-b;
			else
				b = b-a;
		puts(itoa(a, buffer));
		a,b=6;
	}
	f();
	i = (a,b);
	a,b;c,d;
	a[1,2,3];
	a = !b;
	a = -c;
	*/

	/*
	d = &(*g);
	d = &4;

	a = buffer[1] * 4;
	a = buffer * 4;
	a = &8 * d;
	a = 4 * d;

	a = b,&8;

	a = -buffer;

	a = cucu;

	func(c,c);
	*/

	return 0;
}
