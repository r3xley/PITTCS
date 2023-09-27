#include <stdio.h>
#include <stdbool.h>
#include <string.h>

void my_itoa(int value, char *str, int base)
{
    int i = 0;
    bool neg = false;

    // handle base case if it is zero
    if (value == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
    }
    if (value < 0 && base == 10)
    {
        // make neg positive

        neg = true;
        value = -value;
    }

    // cast to unsigned
    if (value < 0 && base != 10)
    {
        neg = true;
        unsigned val = value;
        while (val != 0)
        {
            int temp = val % base;
            if (temp < 10)
            {
                str[i++] = temp + '0'; // convert to ASCII for digits 0-9
            }
            else
            {
                str[i++] = (temp - 10) + 'A'; // convert to ASCII for letters A-F for hexadecimal
            }
            // chop of number
            val = val / base;
        }
    }

    else
    {

        // process the number
        while (value != 0)
        {
            int temp = value % base;
            if (temp < 10)
            {
                str[i++] = temp + '0'; // convert to ASCII for digits 0-9
            }
            else
            {
                str[i++] = (temp - 10) + 'A'; // convert to ASCII for letters A-F for hexadecimal
            }
            // chop of number
            value = value / base;
        }
    }

    // if negative tag the negative sign only if base 10?
    if (neg == true && base == 10)
    {
        str[i++] = '-';
    }

    str[i] = '\0';
    // reverse the string and flip
    int length = strlen(str);
    int start = 0;
    int end = length - 1;
    while (start < end)
    {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}