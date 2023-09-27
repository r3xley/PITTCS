#include <stdio.h>

int my_atoi(const char *str)
{
    int sign = 1;
    int res = 0;
    if (*str == '-')
    {
        sign = -1;
        str++;
    }
    else if (*str == '+')
    {
        str++;
    }
    while (*str >= '0' && *str <= '9')
    {
        int temp = (*str - '0');
        res = (res * 10) + temp;
        str++;
    }
    return sign * res;
}
