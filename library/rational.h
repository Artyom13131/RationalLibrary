#ifndef RATIONAL_H
#define RATIONAL_H

typedef struct {
    long long numerator;
    long long denominator;
} Rational;

typedef struct {
    Rational left;
    Rational right;
} RationalOperands;

typedef enum {
    RATIONAL_OK = 0,
    RATIONAL_ZERO_DENOMINATOR,
    RATIONAL_DIVISION_BY_ZERO,
    RATIONAL_OVERFLOW
} RationalStatus;

RationalStatus rational_create(long long numerator, long long denominator, Rational *result);
RationalStatus rational_normalize(Rational value, Rational *result);
RationalStatus rational_gcd(long long a, long long b, long long *result);
RationalStatus rational_add(RationalOperands operands, Rational *result);
RationalStatus rational_subtract(RationalOperands operands, Rational *result);
RationalStatus rational_multiply(RationalOperands operands, Rational *result);
RationalStatus rational_divide(RationalOperands operands, Rational *result);
RationalStatus rational_compare(RationalOperands operands, int *result);
const char *rational_status_message(RationalStatus status);

#endif