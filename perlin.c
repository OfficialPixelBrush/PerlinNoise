#include <iso646.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
int x = 0;
int y = 0;
int randomNumber = 0;

int abs(int input) {
    if (input < 0) {
        input *= -1;
    }
    return input;
}

int append_int_to_file(int value) {
  // Open the file in append mode ("a")
  FILE* fp = fopen("test.txt", "a");

  // Check if file opened successfully
  if (fp == NULL) {
    return -1; // Error opening file
  }

  // Convert integer to string using sprintf
  char buffer[20]; // Adjust size based on your expected integer range
  int bytes_written = sprintf(buffer, "%d\n", value);

  // Write the string representation of the integer to the file
  if (fputs(buffer, fp) == EOF) {
    fclose(fp);
    return -2; // Error writing to file
  }

  // Close the file
  fclose(fp);

  return bytes_written; // Number of bytes written (excluding newline)
}


/*
This pseudoranom algorithm will use the XOR-Shift algorithm

*/
int pseudoRandom(int input, int max) {
    int seed = 1593247432508327425 % INT32_MAX;
    int sampleBit = 27; // Sample nth bit
    int bitshifts = 6; // Shift n times
    int result = input ^ seed;
    for (int i = 0; i < bitshifts; i++) {
        int temp1;
        int temp2;
        temp1 = result & 1;
        temp2 = (result >> sampleBit) & 1;
        int xorResult = temp1 ^ temp2;
        //printf("%d: %X, %X\n", i, result, xorResult);
        result = result >> 1;
        xorResult = xorResult << (sizeof(int)*8-1);
        result &= ~(1 << (sizeof(int)*8-1));
        result = result | xorResult;
    }
    result = abs(result % max);
    //printf("%d: %d\n", input, result);
    return result;
}

int perlin2d() {

}

int main() {
    printf("Hello, World!\n");
    int i = 0;
    for (i = 0; i < 100; i++) {
        append_int_to_file(pseudoRandom(i, 100));
        //append_int_to_file(rand()%100);
        //append_int_to_file(i);
    }
    return 0;
}