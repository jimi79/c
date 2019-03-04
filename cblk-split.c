#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

const int MAX_KEY_SIZE=4096; 
char files_out[1024][1024]; // 1024 files, and each having a path of 1024 char. should be enough
char file_in[1024];
int key[4096];
int key_size;
int files_out_count;
int block_size = 1; 
int generated_key = 1;

int randint(int n) {
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else { 
		// The idea is to have something evenly randomized
		// for example :
		// RAND_MAX = 2147483647
		// if we ask a random between 0 and 2 (meaning n=3)
		// and RAND_MAX cannot be divided by 3, it means that some values will be generated more often than other, if we just do RAND_MAX % n
		// so, we: 

    // Chop off all of the values that would cause skew...
    long end = RAND_MAX / n; // truncate skew
    end *= n; 
    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);
    return r % n;
  }
} 

int generate_key(int size) {
	srand(time(NULL));
	int i;
	for(i = 0; i < size; i++) {
		key[i] = randint(20)+1;
 		printf("%02x", key[i]);
	}
}

void display_usage(char error[1024]) {
	if (!strcmp(error, "")) { 
		printf("%s\n", error);
	}
	printf("Usage : cblk-split  [if=source_file] of=dest_file [of=next_dest_file] [key=hexa value] [BS=block size]\n");
	printf("\n");
	printf(" Read source_file, with key, to split into dest_files\n");
	printf("\n"); 
	printf(" default values :\n");
	printf("   if: stdin\n"); 
	printf(" 	 key: if not set, then a random key is generated, and returned on stdout.\n");
	printf(" 	 BS: 1 (slowest)\n");
} 

int new_if(char value[]) {
	memcpy(file_in, value, strlen(value));
	file_in[strlen(value)] = '\0';
	return 0;
}

int new_of(char value[]) {
	memcpy(files_out[files_out_count], value, strlen(value));
	files_out[files_out_count][strlen(value)] = '\0';
	files_out_count++; 
	return 0;
}

int set_key(char value[]) {
	int err_high_values = 0;
	int l = strlen(value);
	key_size = l / 2; 
	int m = key_size % files_out_count;
	if (key_size > MAX_KEY_SIZE) {
		fprintf(stderr, "key size is longer than max size (%d)\n", MAX_KEY_SIZE);
		exit(1); 
	}
	if (m == 0) {
		fprintf(stderr, "Key has to not be a multiply of the number of stdout\n");
		return 1;
	} 
	printf("2\n");
	int i;
	for (i = 0; i < key_size; i++) {
		char val[3];
		val[0] = value[i * 2];
		val[1] = value[i * 2 + 1];
		val[2] = '\0';
		int number = (int) strtol(val, NULL, 16);
		//printf("%d\n", number);
		key[i] = number; 
		if (number > 20) {
			err_high_values = 1;
		}
		if (number <= 0) {
			fprintf(stderr, "Key has 0 values\n");
			return 1;
		}
	} 
	if (err_high_values) {
		fprintf(stderr, "Key contain some high values\n");
	} 
	return 0;
}

int set_block_size(char value[]) {
	// have to convert char to int here ...
	block_size = (int) strtol(value, NULL, 10);
	//printf("set BS %d\n", block_size);
	return 0;
}

int process() {
	// open file to read and write here, but by blocks -> how to do it ??? with a buffer for each ??? so that would be some pointer here ? array of char ?
	FILE *in = NULL; 
	unsigned char buffer[255 * block_size]; // 255 = FF, max value for a key indice * blocksize 
	if (file_in[0] == '\0') {
		in = stdin;
	}
	else { 
		if ((in = fopen(file_in, "rb")) == NULL) {
			printf("Cannot open file %s\n", file_in); return 1; };
	} 
	FILE *out[files_out_count];

	int i;
	for (i = 0; i < files_out_count; i++) {
		out[i] = NULL;
		if ((out[i] = fopen(files_out[i], "wb")) == NULL) {
			printf("Cannot open file %s\n", files_out[i]); return 1; };
	}
	int eof = 0;
	int bufsize = 0; 
	int key_pos = 0;
	int files_out_id = 0; 
	while (1) {
		bufsize = key[key_pos] * block_size;
		bufsize = fread(buffer, 1, bufsize, in);
		if (bufsize == 0) {
			break;
		} 
		fwrite(buffer, 1, bufsize, out[files_out_id]);
		key_pos++;
		key_pos = key_pos % key_size;
		files_out_id++;
		files_out_id = files_out_id % files_out_count;
	} 
	for (i = 0; i < files_out_count; i++) {
		fclose(out[i]);
	} 
	if (file_in[0] != '\0') {
		fclose(in);
	} 
}

int parse_parameters(int argc, char *argv[])
{
	int pos_equal; // position of sign = in each string
	int ok = 0;
	int i; 
	char* key;
	for(i = 1; i < argc; i++) {
		char* ptr = strchr(argv[i], '=');
		if (ptr) {
			int index = ptr - argv[i]; // ptr is the absolute address
			char code[index];
			memcpy(code, &argv[i][0], index);
			code[index] = '\0'; 
			char value[strlen(argv[i]) - index];
			memcpy(value, &argv[i][index + 1], strlen(argv[i]) - index);
			if (!strcmp(code, "if")) {
				ok = new_if(value);
			}
			if (!strcmp(code, "of")) {
				ok = new_of(value);
			}
			if (!strcmp(code, "key")) {
				key = malloc(sizeof(char) * (strlen(value) + 1));
				strcpy(key, value);
				generated_key = 0;
			}
			if (!strcmp(code, "BS")) { ok = set_block_size(value); }
			if (ok != 0) {
				return ok;
			}; 
		} else {
			printf("Error, all parameters should be code=value. '%s' isn't\n", argv[i]);
		};
	};
	if (generated_key) {
		key_size = files_out_count+1;
		generate_key(key_size);
	} else { 
		ok = set_key(key);
		free(key); 
		if (ok != 0) { 
			return ok; // something wrong happened
		}
	}
	return 0;
};


int main (int argc, char *argv[])
{ 
	file_in[0] = '\0'; 
	if (argc == 0) {
		display_usage("");
		return 1;
	} 
	int ok = parse_parameters(argc, argv);
	if (ok != 0) { return ok; }; 
	if (files_out_count == 0) {
		display_usage("No output defined !\n");
		return 1;
	} 
	if (key_size == files_out_count) {
		printf("key size equal files in count, not random enough\n");
		return 1;
	}
	process();
	return 0;
};
