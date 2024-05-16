/* TODO: Implement all the functions descripted in this comment.
| Return Type | Function Name | Parameter Type(s) | Description |
|-------------|---------------|-------------------|-------------|
| double      | acos          | double x          | Calculates the arccosine of x. | OK
| double      | asin          | double x          | Calculates the arcsine of x. | Ok
| double      | atan          | double x          | Calculates the arctangent of x. | Ok
| double      | atan2         | double y, double x| Calculates the arctangent of y/x. | Ok
| double      | ceil          | double x          | Returns the smallest integer not less than x. | OK
| double      | cos           | double x          | Calculates the cosine of x. |
| double      | cosh          | double x          | Calculates the hyperbolic cosine of x. |
| double      | exp           | double x          | Calculates e^x, where e is the base of the natural logarithm. |
| double      | fabs          | double x          | Returns the absolute value of x. | OK
| double      | floor         | double x          | Returns the largest integer not greater than x. | OK
| double      | fmod          | double x, double y| Returns the remainder of the division of x by y. |
| double      | frexp         | double x, int* exp| Breaks x into a normalized fraction and an exponent. |
| double      | ldexp         | double x, int exp | Multiplies x by 2 raised to the power of exp. |
| double      | log           | double x          | Calculates the natural logarithm of x. |
| double      | log10         | double x          | Calculates the base 10 logarithm of x. | OK
| double      | modf          | double x, double* intpart | Breaks x into an integral and a fractional part. |
| double      | pow           | double x, double y| Calculates x^y. | OK
| double      | sin           | double x          | Calculates the sine of x. |
| double      | sinh          | double x          | Calculates the hyperbolic sine of x. |
| double      | sqrt          | double x          | Calculates the square root of x. |
| double      | tan           | double x          | Calculates the tangent of x. |
| double      | tanh          | double x          | Calculates the hyperbolic tangent of x. |
 */

#define M_PI 3.14159265358979323846

static const double NAN = 0.0 / 0.0;

double ceil(double x) {
    int i = (int)x;
    return (x > i) ? i + 1 : x;
}

double floor(double x) {
    int i = (int)x;
    return (x < i) ? i - 1 : x;
}

double pow(double x, double y) {
    double result = 1.0;
    for (int i = 0; i < y; i++) {
        result *= x;
    }
    return result;
}

double atan(double x) {
    double result = 0.0;
    double term = x;
    double xsq = x*x;
    for (int i = 0; i < 10; i++) {
        result += term / (2*i+1);
        term *= -xsq;
    }
    return result;
}

double atan2_approx(double y, double x) {
    if (x > 0) {
        return atan(y / x);
    } else if (y >= 0 && x < 0) {
        return atan(y / x) + M_PI;
    } else if (y < 0 && x < 0) {
        return atan(y / x) - M_PI;
    } else if (y > 0 && x == 0) {
        return M_PI / 2;
    } else if (y < 0 && x == 0) {
        return -M_PI / 2;
    } else {
        // x and y are both zero, return undefined
        return NAN;
    }
}

double asin(double x) {
    double result = 0.0;
    double term = x;
    double xsq = x*x;
    for (int i = 0; i < 10; i++) {
        result += term;
        term *= xsq * (2*i+1) / (2*(i+1));
    }
    return result;
}

double acos(double x) {
    // Initialize the sum to 0. This will hold the final result.
    double sum = 0.0;
    // Initialize the term to 1. This will hold the current term in the series.
    double term;
    // Initialize j to 1. This will be used to calculate the factorial in the series.
    double j = 1.0;
    // Loop for the first 10 terms of the series.
    for(int n = 0; n < 10; n++) {
        // Reset the term to 1 for each new term in the series.
        term = 1.0;
        // Calculate the factorial part of the term.
        for(int i = 1; i <= 2*n; i++) {
            term *= (j/i);
            j++;
        }
        // Add the current term to the sum.
        sum += term * pow(x, 2*n+1) / (2*n+1);
    }
    // Return the approximation of the arccosine of x.
    return (M_PI / 2) - sum;
}


double fabs(double x) {
    if (x < 0) {
        return -x;
    } else {
        return x;
    }
}

float fabsf(float x) {
    if (x < 0) {
        return -x;
    }
    return x;
}
long double fabsl(long double x) {
    if (x < 0) {
        return -x;
    }
    return x;
}

double pow10(int n) {
    double result = 1.0;
    if (n >= 0) {
        for (int i = 0; i < n; i++) {
            result *= 10.0;
        }
    } else {
        for (int i = 0; i < -n; i++) {
            result /= 10.0;
        }
    }
    return result;
}

double log10(double x) {
    double lower = 0, upper = x, mid = 0;
    while (upper - lower > 1e-6) { // Continue until the desired precision is reached
        mid = (lower + upper) / 2;
        if (pow10(mid) > x) {
            upper = mid;
        } else {
            lower = mid;
        }
    }
    return mid;
}


