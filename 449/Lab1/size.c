
#include <stdio.h>
#include "size.h"

int main() {
    
    A a;
    B b;
    C c;

   
    printf("The size of struct A is %zu \n", sizeof(a));
    printf("The size of struct B is %zu \n", sizeof(b));
    printf("The size of struct C is %zu \n", sizeof(c));

    return 0;
}
