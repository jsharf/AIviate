#include "approx.h"

// http://en.wikipedia.org/wiki/Trapezoidal_rule
double trap(int *data, int a, int b, int n)
{
    double mult = static_cast<double>(b-a)/(2*n);
    int step = (b-a)/(n);
    int sum = 0;
    for (int i=0; i<b; i+=step)
    {
        sum += data[i] + data[i+step];
    }
    return mult*sum;
}

// general case romberg's method for
// approximating definite integrals
// I did not write this function, I merely modified it
// from its original, at:
// http://en.wikipedia.org/wiki/Romberg%27s_method
// quad is a quadrature method (estimation of numerical integration)
// basic examples include simpson's rule and the trapezoid rule.
// more advanced examples include Guassian quadrature, and Clenshaw–Curtis quadrature
int romberg(int *data, int n, double (*quad)(int*, int, int, int))
{
    double s[n];
    int i,k;
    double var ;
    for (i = 1; i< n; i++)
        s[i] = 1;

    k = 1;
    while((1 << (k + 1)) < n)
    {
        for (i=1; i <=k; i++)
        {
            if (i==1)
            {
                var = s[i];
                s[i] = quad(data, 0, n, (1 << (k - 1)));     // sub-routine approximation
            }                                       // integrated from 0 and 1
                                                    /* pow() is the number of subdivisions*/
            else
            {
                s[k]= ( (1 << 2*(i-1))*s[i-1]-var )/((1 << 2*(i-1)) - 1);

                var = s[i];
                s[i]= s[k];
            }
         }
         k++;
    }
    return s[n-1];
}
