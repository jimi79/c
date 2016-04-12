#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// external 
char files_in[1024][1024]; // 1024 max of 1024 char. i really think i should do pointers sometime. I'll check what is the best for char
// and then how to handle, let's say, a dynamic array of internet on internet
char file_out[1024];
int key[1024] =  {1, 2, 2, 3, 1, 3, 2, 1, 1, 1, 3, 2, 2, 1, 2, 1, 1, 2};
int block_size = 1; //block size
// internal
int key_size = 18;
int files_in_count; 

void display_usage(char error[1024]) {
	if (!strcmp(error, "")) { 
		printf("%s\n", error);
	}
	printf("Usage : regroup if=source_file [if=next_source_file] [of=dest_file] [key=hexa value] [BS=block size]\n");
	printf("\n");
	printf(" Read all source_file, with key, to create dest_file\n");
	printf("\n"); 
	printf(" default values :\n");
	printf("   if : stdout\n"); 
	printf(" 	 key : default key (not recommanded)\n");
	printf(" 	 BS : 1 (best value, slowest\n");
} 

int new_of(char value[]) {
	//printf("new if %s\n", value);
	memcpy(file_out, value, strlen(value));
	file_out[strlen(value)] = '\0';
	return 0;
}

int new_if(char value[]) {
	//printf("new of %s\n", value);
	//printf("length of %d\n", strlen(value)); 
	memcpy(files_in[files_in_count], value, strlen(value));
	files_in[files_in_count][strlen(value)] = '\0';
	files_in_count++;
	return 0;
}

int set_key(char value[]) {
	//printf("set key %s\n", value);
	int m = strlen(value) % 2;
	if (m != 0) { printf("Key has to be an even number of char\n"); return 1; }
	int l = strlen(value);
	key_size = l / 2;
	int i;
	for (i = 0; i < key_size; i++) {
		char val[3];
		val[0] = value[i * 2];
		val[1] = value[i * 2 + 1];
		val[2] = '\0';
		int number = (int)strtol(val, NULL, 16);
		//printf("%d\n", number);
		key[i] = number; 
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
	// open file to read and write here, but by blocks -> how to do it ??? with a buffer for each ??? so that would be some pooutter here ? array of char ?
	FILE *out = NULL; 
	
	unsigned char buffer[255 * block_size]; // 255 = FF, max value for a key outdice * blocksize

	if (file_out[0] == '\0') {
		out = stdout;
	} else 
	{
		if ((out = fopen(file_out, "wb")) == NULL) {
			printf("Cannot open file %s\n", file_out); return 1; };
	}

	FILE *in[files_in_count];

	int i;
	for (i = 0; i < files_in_count; i++) {
		in[i] = NULL;
		if ((in[i] = fopen(files_in[i], "rb")) == NULL) {
			printf("Cannot open file %s\n", files_in[i]); return 1; };
	} 

	int eof = 0;
	int bufsize = 0;

	int key_pos = 0;
	int files_in_id = 0;


	while (1) {
		bufsize = key[key_pos] * block_size;
		bufsize = fread(buffer, 1, bufsize, in[files_in_id]);
		if (bufsize == 0) { break; }  //mmmh, not sure when multiples files in. should store a header maybe in first file to get the original size. or check if bufsize = 0 for all files
		fwrite(buffer, 1, bufsize, out);
		key_pos++;
		key_pos = key_pos % key_size;
		files_in_id++;
		files_in_id = files_in_id % files_in_count;
	}

	for (i = 0; i < files_in_count; i++) {
		fclose(in[i]);
	} 

	if (file_out[0] != '\0') {
		fclose(out);
	}


}

int parse_parameters(int argc, char *argv[])
{
	int pos_equal; // position of sign = in each string
	int i;

	for(i = 1; i < argc; i++) {
		char *ptr = strchr(argv[i], '=');
		if (ptr) {
			int index = ptr - argv[i];
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
	file_out[0] = '\0';
	int ok = parse_parameters(argc, argv);
	if (ok != 0) { return ok; };

	if (argc == 0) {
		display_usage("");
		return 1;
	}

	if (files_in_count == 0) {
		display_usage("No input defined !\n");
		return 1;
	}

	if (key_size == files_in_count) {
		printf("key size equal files in count, not random enough\n");
		return 1;
	} 

	process(); 
	return 0;
};
