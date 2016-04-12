#include <stdio.h>

void calcadrint(int adrext, int nbvol, int* key, int sizekey, int sum, int* vol, int* adrint) {
	int save_adrext = adrext;
	int tempadrint = adrext / (sum * nbvol) * sum;
	adrext = adrext % (sum * nbvol); 
	int i; int vid = 0;
	int shiftvol[nbvol];

	for (i = 0; i < nbvol; i++) {
		shiftvol[i] = 0;
	}

	i = 0;
	while (adrext >= key[i]) {
		adrext = adrext - key[i];
		shiftvol[vid] += key[i];
		vid = (vid + 1) % nbvol;
		//printf("--%d---%d---%d---%d\n", adrext, vid, key[i], i);
		i = (i + 1) % (sizekey);
	}
	tempadrint = tempadrint + adrext + shiftvol[vid];

	*vol = vid;
	*adrint = tempadrint;
	return;
}


int main() { 
	//test();
	//return 0;
	int key[10] = {1,2,5,4,23,3,4,54,2,3}; // TODO : use a string to represent that in hexa maybe, like 0102030C05 for that example
	int sizekey = sizeof(key) / sizeof(key[0]);
	int nbvol = 3;
	int sum = 0;
	int i; 
	for (i = 0; i < sizekey; i++) {
		sum += key[i];
	} 
	int vol; int adrext;
	for (i = 0; i < 5000; i++) {
		calcadrint(i, nbvol, key, sizekey, sum, &vol, &adrext); 
		printf("%d;%d;%d\n", i, vol, adrext); 

	}
}
