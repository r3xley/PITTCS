
/*
 * pointer.c - Source file with your solutions to the Lab.
 *             This is the file you will hand in to your instructor.
 */
#ifndef COMMON_H
#include "common.h"
#endif

/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

#if 0
You will provide your solution to this lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code
  must conform to the following style:

  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Function arguments and local variables (no global variables).
  2. Integer literals.

  Some of the problems restrict the set of allowed operators.

  You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions (unless otherwise stated).
  4. Use any operators not "allowed" at the top of the method you are writing.

  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has *undefined* behavior when shifting by a negative amount or an amount
     greater than or equal to the number of bits in the value being shifted.
     e.g. For x >> n, shifts by n < 0 or n >= *size of x* are undefined
     e.g. if x is a 32-bit int, shifts by >= 32 bits are undefined
     Undefined means you do not know what result you will get from the operation.
#endif

/*
 * STEP 2: Modify the following functions according the coding rules.
 */

/*
 * Return the size of an integer in bytes.
 *
 *  Calculate the size of an int using only arithmetic and casts!
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 *   Any non arithmetic operatror
 */
int intSize() {
//claculate the sixe of an int by pointer arithmatic 
  int intNumber;  
  int *ptr1, *ptr2;

  ptr1 = &intNumber; // Get a pointer to the integer variable
  ptr2 = ptr1 + 1; // 1 * sizeof int

  
  // Calculate the difference
  return (char*)ptr2 - (char*)ptr1;
}


/*
 * Swap the 2 ints.
  * Write code that swaps the contents of the 2 ints
 *   Kudos if you do it without a temp variable 
 *   ( But you need to know your bitwises! :)
 *   
 */
void swapInts(int *i1, int *i2){
// swaps the pointer using a temp variable  as a placeholder
    int temp = *i1;  
    //swap
    *i1 = *i2;     
    *i2 = temp;      
}


/*
 * Write the 4 Bytes that compose the integer "value" (little-endian)
 *   into the char array "array". They should be written 
 *   in big-endian byte order.
 *   Assume the char* points at a memory location with 4B available
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 */
void serializeBE(unsigned char *array, int value) {
// Use pointer arithmetic to access individual bytes of the integer and write into the array 
    unsigned char *ptr = (unsigned char *)&value;

    // Write each byte in big endian 
    *(array) = *(ptr + 3 );  
    *(array + 1) = *(ptr + 2);
    *(array + 2) = *(ptr + 1);
    *(array + 3) = *(ptr );  
}

/*
 * Write the 4 Bytes in the char array "array" (big-endian byte order) 
 *   into the integer "value" in little-endian byte order
 *   Assume the char* points at a memory location with 4B available
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 */
void deserializeBE(int* value, const unsigned char *array) {
// Use pointers to write bytes from array into value (similar to above)
unsigned char *ptr = (unsigned char *)value;

    // write bytes into value
    *(ptr) = *(array + 3);  // Least significant byte (LSB)
    *(ptr + 1) = *(array + 2);
    *(ptr + 2) = *(array + 1);
    *(ptr + 3) = *(array);  // Most significant byte (MSB)
}

/*
 * Return 1 if ptr points to an element within the specified intArray, 0 otherwise.
 * Pointing anywhere in the array is fair game, ptr does not have to
 * point to the beginning of an element. Check the spec for examples if you are 
 * confused about what this method is determining.
 * size is the number of elements of intArray. Assume size != 0.
 *
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 */
int withinArray(int *intArray, int size, int *ptr) {
  //calculate size of array and check if ptr lies inside. If yes, return 1, else return 0
    int *first = intArray;          
    int *last = intArray + size;      

    if (ptr >= first && ptr < last) {
        return 1;  
    } else {
        return 0;  
    }
}

/*
 * In C characters are are terminated by the null character ('\0')
 * given a pointer to the start of the string return the length of this string.
 * (The null character is not counted as part of the string length.)
 *
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 */
int stringLength(char *s) {
  //make the length 0
    int length = 0;
    //loop until value is a null terminator + increment length 
    while(*s != '\0'){
        
        length++;
        s++;
    }
    
    return length;
}

/*
 * Returns the length of the initial portion of str1 which consists only of characters that are part of str2.
 * The search does not include the terminating null-characters of either strings, but ends there.
 * 
 * Example, stringSpan("abcdefgh", "abXXcdeZZh"); // returns 5
 *  
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 */
int stringSpan(char * str1, char * str2) {
//loops to find the longest substring  that is also contained in str2
    int length = 0;

    while (*str1 != '\0') {
        char *ptr2 = str2; // Reset str2Ptr to the beginning of str2 for each character in str1

        // Check if the current character in str1 exists in str2
        while (*ptr2 != '\0') {
            if (*str1 == *ptr2) {
                length++;
                break; // Break out of the inner loop if a match is found
            }
            ptr2++;
        }

        // If the character in str1 is not found in str2, stop searching
        if (*ptr2 == '\0') {
            break;
        }

        str1++;
    }

    return length;  
}

/*
 * Selection sort is a sorting algorithim that works by partitioning the array into
 * a sorted section and unsorted section. Then it repeatedly selects the minimum element
 * from the unsorted section and moves it to the end of the sorted section.
 *
 * So the pseudo-code might look something like this:
 * arr - an array
 * n - the length of arr
 *
 * for i = 0 to n - 1
 *   minIndex = i
 *   for  j = i + 1 to n
 *       if arr[minIndex] > arr[j]
 *           minIndex = j
 *       end if
 *   end for
 *   Swap(arr[i], arr[minIndex])
 * end for
 *
 * Implement selection sort below, it might be helpful to use the swapInts function you
 * defined earlier.
 *
 * DISALLOWED:
 *   Pointer operators: [] (Array Indexing Operator)
 */
void selectionSort(int *array, int arrLength) {
//selection sort which take min element and moves to end. Follows sudo-code above but with pointers    
    for (int i = 0; i < arrLength - 1; i++) {
   	 int *minPtr = array + i;  // Initialize minPtr to the current element
    	 for (int j = i + 1; j < arrLength; j++) {
      		if (*minPtr > *(array + j)) {
        	minPtr = array + j;  // Update minPtr if a smaller element is found
      }
    }
    // Swap the elements at minPtr and arr[i]
    swapInts(minPtr, array + i);
  }
    
}

