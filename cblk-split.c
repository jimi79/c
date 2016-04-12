#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// external 
char files_out[1024][1024]; // 1024 max of 1024 char. i really think i should do pointers sometime. I'll check what is the best for char
// and then how to handle, let's say, a dynamic array of internet on internet
char file_in[1024];
int key[1024] =  {1, 2, 2, 3, 1, 3, 2, 1, 1, 1, 3, 2, 2, 1, 2, 1, 1, 2};
int block_size=1; //block size
// internal
int key_size = 18;
int files_out_count; 

void display_usage(char error[1024]) {
	if (!strcmp(error, "")) { 
		printf("%s\n", error);
	}
	printf("Usage : split  [if=source_file] of=dest_file [of=next_dest_file] [key=hexa value] [BS=block size]\n");
	printf("\n");
	printf(" Read source_file, with key, to split into dest_files\n");
	printf("\n"); 
	printf(" default values :\n");
	printf("   if : stdin\n"); 
	printf(" 	 key : default key (not recommanded). If the key isn't set, then it's returned.\n");
	printf(" 	 BS : 1 (best value, slowest\n");
} 

int new_if(char value[]) {
	//printf("new if %s\n", value);
	memcpy(file_in, value, strlen(value));
	file_in[strlen(value)] = '\0';
	return 0;
}

int new_of(char value[]) {
	//printf("new of %s\n", value);
	//printf("length of %d\n", strlen(value)); 
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
	if (m == 0) { fprintf(stderr, "Key has to not be a multiply of the number of stdout\n"); return 1; }

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
		//buffer[bufsize] = '\0'; // only used if i print for debug purpose
		//printf(buffer);
		if (bufsize == 0) { break; } 
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
	int i;

	for(i = 1; i < argc; i++) {
		char *ptr = strchr(argv[i], '=');
		if (ptr) {
			int index = ptr - argv[i]; // ptr is the absolute address
			char code[index];
			memcpy(code, &argv[i][0], index);
			code[index] = '\0'; 
			char value[strlen(argv[i]) - index];
			memcpy(value, &argv[i][index + 1], strlen(argv[i]) - index);
			//printf("%s=%s\n", code, value); 
			int ok;
			if (!strcmp(code, "if")) { ok = new_if(value); }
			if (!strcmp(code, "of")) { ok = new_of(value); }
			if (!strcmp(code, "key")) { ok = set_key(value); }
			if (!strcmp(code, "BS")) { ok = set_block_size(value); }
			if (ok != 0) { return ok; }; 
		} else
		{ printf("Error, all parameters should be code=value. '%s' isn't\n", argv[i]); };
	};
	return 0;
};


int main (int argc, char *argv[])
{ 
	file_in[0] = '\0';

	int ok = parse_parameters(argc, argv);
	if (ok != 0) { return ok; };
	
	if (argc == 0) {
		display_usage("");
		return 1;
	}

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
