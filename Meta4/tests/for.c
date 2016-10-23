int main(int argc, char** argv) {
	int x;
	int y;
	int i;
	int j;
	char buf[20];

	x = atoi(argv[1]);
	y = atoi(argv[2]);

	for(i = 0; i <= x; i = i+1) {
		for(j = 0; j <= y; j = j+1) {
			/*buf[0] = 'c'; */
			puts(itoa(i, buf));
		}
	}

	return 0;
}