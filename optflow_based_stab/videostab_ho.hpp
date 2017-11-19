//
//  videostab_ho.hpp
//  NR
//
//  Created by Ito Yuichi on 2017/11/18.
//  Copyright © 2017年 Ito Yuichi. All rights reserved.
//

#ifndef videostab_ho_hpp
#define videostab_ho_hpp

#include <stdio.h>

#endif /* videostab_ho_hpp */

#include <math.h>

double huber(double a, double b, double th);



double huber(double input, double base, double th){
    //            double th=0.1;
    if(fabs(input-base) < th){
        return input;
    }
    else{
        if(input>base){
            return base+th;
        }else{
            return base-th;
        }

    }
}


/*
num = 6
t = zeros(num, 1);
D = 64.7;
U = 5.81E3;
r = 108E-3;
ne = 1.8E14;
for i = 1:num
t(i) = i * 20E-6 / num;
Ne(i) = log(ne / (4. * pi * D * t(i))^1.5 * exp(-(r - U * t(i)) * (r - U * t(i))
                                                / (4. * D * t(i)))) + rand() * 10;
end
 
 
% non-linear least squares
r = 108E-3;
% initial estimation
nep = log(1.75E14);
D = 5.6;
U = 5.50E3;
x = [nep; D; U];
–4–
% Calculating Delta f
count = 0;
 
while count < 20;
 for i = 1:num
    ENe(i) = nep - 1.5 * log(4. * pi * D * t(i)) - (r - U * t(i)) * (r - U * t(i))/ (4. * D * t(i));
    Df(i, 1) = Ne(i) - ENe(i);
    J(i, 1) = 1.;
    J(i, 2) = -1.5 / D + (r - U * t(i)) * (r - U * t(i)) / (4. * D * D * t(i));
    J(i, 3) = (r - U * t(i)) / (2. * D);
 end
 str = sprintf(’%f, %f, %f’, x(1), x(2), x(3));
 disp(str);
 Dx = inv(J’ * J) * (J’ * Df);
 x = x + Dx;
 nep = x(1);
 D = x(2);
 U = x(3);
 count = count + 1;
end
 
for i = 1:num
NE(i) = nep - 1.5 * log(4. * pi * D * t(i)) - (r - U * t(i)) * (r - U * t(i))
/ (4. * D * t(i));
end
plot(t, Ne, t, NE);
*/
