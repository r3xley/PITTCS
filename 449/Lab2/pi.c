#include <stdlib.h>
#include <math.h>
#include <stdio.h>
double estimate_pi(int number_of_samples)
{
    int num_hits = 0;
    double radius = 1;
    for (int i = 0; i < number_of_samples; ++i)
    {
        double x = (double)rand() / RAND_MAX * (2 * radius) - radius;
        double y = (double)rand() / RAND_MAX * (2 * radius) - radius;

        if ((x * x) + (y * y) <= (radius * radius))
        {
            num_hits++;
        }
    }
    double pi = 4.0 * ((double)num_hits / (double)number_of_samples);
    return (pi);
}
