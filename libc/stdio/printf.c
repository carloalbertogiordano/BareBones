#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

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

double fabs(double x) {
    if (x < 0) {
        return -x;
    } else {
        return x;
    }
}

char* itoa(int value, char* str, int base) {
    char* ptr = str, *ptr1 = str, tmp_char;
    int tmp_value;

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "fedcba9876543210123456789abcdef"[15 + (tmp_value - value * base)];
    } while (value);

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
    return str;
}





static bool print(const char* data, size_t length) {
	const unsigned char* bytes = (const unsigned char*) data;
	for (size_t i = 0; i < length; i++)
		if (putchar(bytes[i]) == EOF)
			return false;
	return true;
}

int print_char(va_list parameters) {
    char c = (char) va_arg(parameters, int);
    return print(&c, sizeof(c));
}

int print_string(va_list parameters) {
    const char* str = va_arg(parameters, const char*);
    return print(str, strlen(str));
}

int print_int(va_list parameters) {
    int i = va_arg(parameters, int);
    char str[12]; // Buffer big enough for an integer
    itoa(i, str, 10);
    return print(str, strlen(str));
}

int print_unsigned_int(va_list parameters) {
    unsigned int u = va_arg(parameters, unsigned int);
    char str[12]; // Buffer big enough for an unsigned integer
    itoa(u, str, 10);
    return print(str, strlen(str));
}

int print_hex(va_list parameters) {
    int x = va_arg(parameters, int);
    char str[12]; // Buffer big enough for an integer in hexadecimal
    itoa(x, str, 16);
    return print(str, strlen(str));
}

int print_oct(va_list parameters) {
    int o = va_arg(parameters, int);
    char str[12]; // Buffer big enough for an integer in octal
    itoa(o, str, 8);
    return print(str, strlen(str));
}

int print_ptr(va_list parameters) {
    void* p = va_arg(parameters, void*);
    char str[20]; // Buffer big enough for a pointer
    itoa(*(int *)p, str, 16);
    return print(str, strlen(str));
}

int print_percent(va_list parameters) {
    (void ) parameters;
    return print("%", 1);
}

int print_float(va_list parameters) {
    double f = va_arg(parameters, double);
    int integer_part = (int)f;
    double fractional_part = f - integer_part;

    char str[64]; // Buffer big enough for a floating point number
    itoa(integer_part, str, 10);

    // Add decimal point
    int len = strlen(str);
    str[len] = '.';
    str[len + 1] = '\0';

    // Convert fractional part to string
    for (int i = 0; i < 6; i++) { // Print up to 6 decimal places
        fractional_part *= 10;
        int digit = (int)fractional_part;
        str[len + 1 + i] = '0' + digit;
        fractional_part -= digit;
    }

    return print(str, strlen(str));
}

int print_exp(va_list parameters) {
    double e = va_arg(parameters, double);
    int exponent = (int)log10(fabs(e));
    double mantissa = e / pow10(exponent);

    char str[64]; // Buffer big enough for a floating point number
    itoa((int)mantissa, str, 10);

    // Add decimal point
    int len = strlen(str);
    str[len] = '.';
    str[len + 1] = '\0';

    // Convert fractional part to string
    double fractional_part = mantissa - (int)mantissa;
    for (int i = 0; i < 6; i++) { // Print up to 6 decimal places
        fractional_part *= 10;
        int digit = (int)fractional_part;
        str[len + 1 + i] = '0' + digit;
        fractional_part -= digit;
    }

    // Add exponent part
    len = strlen(str);
    str[len] = 'e';
    str[len + 1] = '+';
    itoa(exponent, str + len + 2, 10);

    return print(str, strlen(str));
}

int print_g(va_list parameters) {
    double g = va_arg(parameters, double);
    int exponent = (int)log10(fabs(g));

    if (exponent < -4 || exponent >= 6) {
        // Use exponential notation
        return print_exp(parameters);
    } else {
        // Use fixed-point notation
        return print_float(parameters);
    }
}

int print_E(va_list parameters) {
    double e = va_arg(parameters, double);
    int integer_part = (int)e;
    double fractional_part = e - integer_part;

    char str[64]; // Buffer big enough for a floating point number
    itoa(integer_part, str, 10);

    // Add decimal point
    int len = strlen(str);
    str[len] = '.';
    str[len + 1] = '\0';

    // Convert fractional part to string
    for (int i = 0; i < 6; i++) { // Print up to 6 decimal places
        fractional_part *= 10;
        int digit = (int)fractional_part;
        str[len + 1 + i] = '0' + digit;
        fractional_part -= digit;
    }

    // Add exponent part
    len = strlen(str);
    str[len] = 'E';
    str[len + 1] = '+';
    itoa(0, str + len + 2, 10); // Assuming the exponent is 0 for simplicity

    return print(str, strlen(str));
}

int print_G(va_list parameters) {
    double g = va_arg(parameters, double);
    int exponent = (int)log10(fabs(g));

    if (exponent < -4 || exponent >= 6) {
        // Use exponential notation
        return print_E(parameters);
    } else {
        // Use fixed-point notation
        return print_float(parameters);
    }
}

int printf(const char* restrict format, ...) {
    va_list parameters;
    va_start(parameters, format);

    int written = 0;

    while (*format != '\0') {
        size_t maxrem = INT_MAX - written;

        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%')
                format++;
            size_t amount = 1;
            while (format[amount] && format[amount] != '%')
                amount++;
            if (maxrem < amount) {
                // TODO: Set errno to EOVERFLOW.
                return -1;
            }
            if (!print(format, amount))
                return -1;
            format += amount;
            written += amount;
            continue;
        }

        format++; // skip the '%'

        switch (*format) {
            case 'c': {
                written += print_char(parameters);
                break;
            }
            case 's': {
                written += print_string(parameters);
                break;
            }
            case 'd': {
                written += print_int(parameters);
                break;
            }
            case 'f': {
                    written += print_float(parameters);
                    break;
                }
            case 'g': {
                written += print_g(parameters);
                break;
            }
            case 'E': {
                written += print_E(parameters);
                break;
            }
            case 'G': {
                written += print_G(parameters);
                break;
            }
            default: {
                // TODO: Handle other types.
                break;
            }
        }

        format++;
    }

    va_end(parameters);
    return written;
}
