#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "interpolation.h"

using namespace alglib;
#include<time.h>

/*
int main(int argc, char **argv)
{
    //
    // This example demonstrates polynomial fitting.
    //
    // Fitting is done by two (M=2) functions from polynomial basis:
    //     f0 = 1
    //     f1 = x
    // with simple constraint on function value
    //     f(0) = 0
    // Basically, it just a linear fit; more complex polynomials may be used
    // (e.g. parabolas with M=3, cubic with M=4), but even such simple fit allows
    // us to demonstrate polynomialfit() function in action.
    //
    // We have:
    // * x      set of abscissas
    // * y      experimental data
    // * xc     points where constraints are placed
    // * yc     constraints on derivatives
    // * dc     derivative indices
    //          (0 means function itself, 1 means first derivative)
    //
    real_1d_array x = "[1.0,1.0]";
    real_1d_array y = "[0.9,1.1]";
//    real_1d_array x = "[0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0]";
//    real_1d_array y = "[0.00,0.05,0.26,0.32,0.33,0.43,0.60,0.60,0.77,0.98,1.02]";
    real_1d_array w = "[1,1]";
    real_1d_array xc = "[0]";
    real_1d_array yc = "[0]";
    integer_1d_array dc = "[0]";
    double t = 2;
    ae_int_t m = 3;
    ae_int_t info;
    barycentricinterpolant p;
    polynomialfitreport rep;
    double v;
    clock_t st=clock();
    polynomialfitwc(x, y, w, xc, yc, dc, m, info, p, rep);
    clock_t en=clock();
    printf("time: %f\n",double(en-st)/ CLOCKS_PER_SEC);
    
    v = barycentriccalc(p, t);
    printf("%.2f\n", double(v)); // EXPECTED: 2.000
    return 0;
}
 */

/*
int main(int argc, char **argv)
{
    //
    // This example demonstrates polynomial fitting.
    //
    // Fitting is done by two (M=2) functions from polynomial basis:
    //     f0 = 1
    //     f1 = x
    // Basically, it just a linear fit; more complex polynomials may be used
    // (e.g. parabolas with M=3, cubic with M=4), but even such simple fit allows
    // us to demonstrate polynomialfit() function in action.
    //
    // We have:
    // * x      set of abscissas
    // * y      experimental data
    //
    // Additionally we demonstrate weighted fitting, where second point has
    // more weight than other ones.
    //
    real_1d_array x = "[0.0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0]";
    real_1d_array y = "[0.00,0.05,0.26,0.32,0.33,0.43,0.60,0.60,0.77,0.98,1.02]";
    ae_int_t m = 2;
    double t = 1.1;
    ae_int_t info;
    barycentricinterpolant p;
    polynomialfitreport rep;
    double v;
    
    //
    // Fitting without individual weights
    //
    // NOTE: result is returned as barycentricinterpolant structure.
    //       if you want to get representation in the power basis,
    //       you can use barycentricbar2pow() function to convert
    //       from barycentric to power representation (see docs for
    //       POLINT subpackage for more info).
    //
    
    clock_t st=clock();
    
    polynomialfit(x, y, m, info, p, rep);
    v = barycentriccalc(p, t);
    printf("%.2f\n", double(v)); // EXPECTED: 2.011
    
    clock_t en=clock();
    printf("time: %f\n",double(en-st)/ CLOCKS_PER_SEC);

    
    //
    // Fitting with individual weights
    //
    // NOTE: slightly different result is returned
    //
    real_1d_array w = "[1,1.414213562,1,1,1,1,1,1,1,1,1]";
    real_1d_array xc = "[]";
    real_1d_array yc = "[]";
    integer_1d_array dc = "[]";
    polynomialfitwc(x, y, w, xc, yc, dc, m, info, p, rep);
    v = barycentriccalc(p, t);
    printf("%.2f\n", double(v)); // EXPECTED: 2.023
    return 0;
}
*/

int main(int argc, char **argv)
{
    //
    // In this example we demonstrate penalized spline fitting of noisy data
    //
    // We have:
    // * x - abscissas
    // * y - vector of experimental data, straight line with small noise
    //
    
    
    //real_1d_array x = "[0.00,0.10,0.20,0.30,0.40,0.50,0.60,0.70,0.80,0.90]";
    //real_1d_array y = "[0.10,0.00,0.30,0.40,0.30,0.40,0.62,0.68,0.75,0.95]";
    
    std::vector<double> X(5), Y(5);
    X[0]=0.1;
    X[1]=0.4;
    X[2]=1.2;
    X[3]=1.8;
    X[4]=2.0;
    Y[0]=0.1;
    Y[1]=0.7;
    Y[2]=0.4;
    Y[3]=1.1;
    Y[4]=1.9;
    real_1d_array x, y;
    x.setcontent(X.size(), &(X[0]));
    y.setcontent(Y.size(), &(Y[0]));
    
    
    ae_int_t info;
    double v;
    spline1dinterpolant s;
    spline1dfitreport rep;
    double rho;
    
    //
    // Fit with VERY small amount of smoothing (rho = -5.0)
    // and large number of basis functions (M=50).
    //
    // With such small regularization penalized spline almost fully reproduces function values
    //
    clock_t st=clock();
    
    rho = 10.0;
    spline1dfitpenalized(x, y, 5, rho, info, s, rep);
    
    clock_t en=clock();
    printf("time: %f\n",double(en-st)/ CLOCKS_PER_SEC);

    printf("%d\n", int(info)); // EXPECTED: 1
    for(float i=0; i<1; i=i+0.1){
        v = spline1dcalc(s, i);
        printf("%.2f, %.2f\n", i,double(v)); // EXPECTED: 0.10
    }
    

    
    //
    // Fit with VERY large amount of smoothing (rho = 10.0)
    // and large number of basis functions (M=50).
    //
    // With such regularization our spline should become close to the straight line fit.
    // We will compare its value in x=1.0 with results obtained from such fit.
    //
    rho = +10.0;
    spline1dfitpenalized(x, y, 50, rho, info, s, rep);
    printf("%d\n", int(info)); // EXPECTED: 1
    v = spline1dcalc(s, 1.0);
    printf("%.2f\n", double(v)); // EXPECTED: 0.969
    
    //
    // In real life applications you may need some moderate degree of fitting,
    // so we try to fit once more with rho=3.0.
    //
    rho = +3.0;
    spline1dfitpenalized(x, y, 50, rho, info, s, rep);
    printf("%d\n", int(info)); // EXPECTED: 1
    return 0;
}

