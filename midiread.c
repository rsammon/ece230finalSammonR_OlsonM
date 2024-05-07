#pragma warning(disable:4996)
#include <stdio.h>
#include <stdlib.h>
int main() {
	FILE* ptr;
	ptr = fopen("Untitled.mid", "rb");
	if (ptr == NULL) {
		fprintf(stderr, "Failed to open file\n");
		return 1;
	}

	fseek(ptr, 0, SEEK_END);
	size_t size = ftell(ptr);
	rewind(ptr);

	unsigned char *buffer = (unsigned char*)calloc(size, sizeof(unsigned char));
	if (buffer == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		return 1;
	}

	fread(buffer, sizeof(buffer), size, ptr);
	for (int i = 0; i < size; i++)
		printf("%02x ", buffer[i]);
	fclose(ptr);
}

