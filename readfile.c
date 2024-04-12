// readfile.c
#include "readfile.h"
#include <stdio.h>
#include <stdlib.h>

static FILE *file = NULL;

int open_file(const char *filename) {
    file = fopen(filename, "r");
    if (file == NULL) {
    	perror("Error opening file");
        return -1; 
    }
    return 0; 	
}

int read_int(int *value) {
    if (fscanf(file, "%d", value) != 1) {
        return -1;
    }
    return 0; 
}

int read_float(float *value) {
    if (fscanf(file, "%f", value) != 1) {
        return -1;
    }
    return 0;
}

int read_string(char *str, int maxLen) {
    if (fscanf(file, "%63s", str) != 1) { 
        return -1;
    }
    return 0;
}

void close_file() {
    if (file != NULL) {
        fclose(file);
        file = NULL;
    }
}

