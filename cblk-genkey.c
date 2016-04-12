#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int randint(int n) {
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {
    // Chop off all of the values that would cause skew...
    long end = RAND_MAX / n; // truncate skew
    //assert (end > 0L);
    end *= n;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % n;
  }
} 

int main(int argc, char *argv[]) {
	long unsigned int len;
	if (argc == 1) {
		len = 255;
	}
	else { 
		len = (int) strtol(argv[1], NULL, 10);
	}

	srand(time(NULL));	
	int i;
	for (i = 0; i < len; i++) {
		printf("%02x", randint(10) + 1); // only from 1 to 10, greater is too dangerous
	}
}
