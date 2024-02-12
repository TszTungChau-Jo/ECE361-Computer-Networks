#include <stdio.h>
#include <stdlib.h>
/*
https://stackoverflow.com/questions/18857056/c11-generic-how-to-deal-with-string-literals
https://stackoverflow.com/questions/479207/how-to-achieve-function-overloading-in-c
https://stackoverflow.com/questions/17302913/generics-for-multiparameter-c-functions-in-c11/25715458#25715458
  */
// Function to convert a signed decimal integer to a string
char* int_to_string(int value) {
    char* result = (char*)malloc(20);  // Allocate enough memory for the result
    sprintf(result, "%d", value);
    return result;
}

// Function to convert an unsigned decimal integer to a string
char* unsigned_int_to_string(unsigned int value) {
    char* result = (char*)malloc(20);  // Allocate enough memory for the result
    sprintf(result, "%u", value);
    return result;
}

// Function to convert an unsigned octal integer to a string
char* octal_to_string(unsigned int value) {
    char* result = (char*)malloc(20);  // Allocate enough memory for the result
    sprintf(result, "%o", value);
    return result;
}

// Function to convert an unsigned hexadecimal integer to a string (lowercase)
char* hex_to_string(unsigned int value) {
    char* result = (char*)malloc(20);  // Allocate enough memory for the result
    sprintf(result, "%x", value);
    return result;
}

// Function to convert an unsigned hexadecimal integer to a string (uppercase)
char* hex_upper_to_string(unsigned int value) {
    char* result = (char*)malloc(20);  // Allocate enough memory for the result
    sprintf(result, "%X", value);
    return result;
}

// Function to convert a decimal floating-point number to a string (lowercase)
char* float_to_string(float value) {
    char* result = (char*)malloc(30);  // Allocate enough memory for the result
    sprintf(result, "%f", value);
    return result;
}

// Function to convert a decimal floating-point number to a string (uppercase)
char* float_upper_to_string(float value) {
    char* result = (char*)malloc(30);  // Allocate enough memory for the result
    sprintf(result, "%F", value);
    return result;
}

// Function to convert a floating-point number to scientific notation (lowercase)
char* scientific_to_string(float value) {
    char* result = (char*)malloc(30);  // Allocate enough memory for the result
    sprintf(result, "%e", value);
    return result;
}

// Function to convert a floating-point number to scientific notation (uppercase)
char* scientific_upper_to_string(float value) {
    char* result = (char*)malloc(30);  // Allocate enough memory for the result
    sprintf(result, "%E", value);
    return result;
}

// Function to convert a floating-point number to the shortest representation
char* shortest_float_to_string(float value) {
    char* result = (char*)malloc(30);  // Allocate enough memory for the result
    sprintf(result, "%g", value);
    return result;
}

// Function to convert a floating-point number to the shortest representation (uppercase)
char* shortest_float_upper_to_string(float value) {
    char* result = (char*)malloc(30);  // Allocate enough memory for the result
    sprintf(result, "%G", value);
    return result;
}

// Function to convert a hexadecimal floating-point number to a string (lowercase)
char* hex_float_to_string(float value) {
    char* result = (char*)malloc(30);  // Allocate enough memory for the result
    sprintf(result, "%a", value);
    return result;
}

// Function to convert a hexadecimal floating-point number to a string (uppercase)
char* hex_float_upper_to_string(float value) {
    char* result = (char*)malloc(30);  // Allocate enough memory for the result
    sprintf(result, "%A", value);
    return result;
}

// Function to convert a pointer address to a string
char* pointer_to_string(void* value) {
    char* result = (char*)malloc(20);  // Allocate enough memory for the result
    sprintf(result, "%p", value);
    return result;
}

// Function to convert a character to a string
char* char_to_string(char value) {
    char* result = (char*)malloc(2);  // Allocate enough memory for the result
    sprintf(result, "%c", value);
    return result;
}

// Function to convert a string to a string (no conversion needed)
char* string_to_string(const char* value) {
    return value;  // Duplicate the input string
}


// Define the print macro
#define print(s) puts(str(s))
#define str(x) _Generic((x), \
    char*: string_to_string, \
    const char*: string_to_string, \
    int: int_to_string, \
    unsigned int: unsigned_int_to_string, \
    unsigned int[sizeof(x)]: octal_to_string, \
    float: float_to_string, \
    char: char_to_string, \
    const char[sizeof(x)]: string_to_string, \
    default: pointer_to_string \
)(x)