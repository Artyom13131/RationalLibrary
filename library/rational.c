#include <limits.h>
#include "rational.h"

static int checked_add(long long a, long long b, long long *r)
{
    if (b > 0 && a > LLONG_MAX - b) return 1;
    if (b < 0 && a < LLONG_MIN - b) return 1;
    *r = a + b;
    return 0;
}

static int checked_sub(long long a, long long b, long long *r)
{
    if (b > 0 && a < LLONG_MIN + b) return 1;
    if (b < 0 && a > LLONG_MAX + b) return 1;
    *r = a - b;
    return 0;
}

static int checked_mul(long long a, long long b, long long *r)
{
    if (a > 0) {
        if (b > 0) { if (a > LLONG_MAX / b) return 1; }
        else { if (b < LLONG_MIN / a) return 1; }
    } else {
        if (b > 0) { if (a < LLONG_MIN / b) return 1; }
        else { if (a != 0 && b < LLONG_MAX / a) return 1; }
    }
    *r = a * b;
    return 0;
}

RationalStatus rational_gcd(long long a, long long b, long long *result)
{
    if (a == LLONG_MIN || b == LLONG_MIN)
        return RATIONAL_OVERFLOW;
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b != 0) {
        long long t = a % b;
        a = b;
        b = t;
    }
    *result = a;
    return RATIONAL_OK;
}

RationalStatus rational_normalize(Rational value, Rational *result)
{
    long long a = value.numerator;
    long long b = value.denominator;
    long long g;
    RationalStatus status;

    if (b == 0) return RATIONAL_ZERO_DENOMINATOR;
    if (a == 0) {
        result->numerator = 0;
        result->denominator = 1;
        return RATIONAL_OK;
    }
    if (a == LLONG_MIN || b == LLONG_MIN) return RATIONAL_OVERFLOW;
    if (b < 0) { a = -a; b = -b; }

    status = rational_gcd(a, b, &g);
    if (status != RATIONAL_OK) return status;

    result->numerator = a / g;
    result->denominator = b / g;
    return RATIONAL_OK;
}

RationalStatus rational_create(long long numerator, long long denominator, Rational *result)
{
    Rational value;
    value.numerator = numerator;
    value.denominator = denominator;
    return rational_normalize(value, result);
}

static RationalStatus add_or_subtract(RationalOperands operands, int subtract, Rational *result)
{
    Rational x, y, raw;
    long long g, xd, yd, t1, t2;
    RationalStatus status;

    status = rational_normalize(operands.left, &x);
    if (status != RATIONAL_OK) return status;
    status = rational_normalize(operands.right, &y);
    if (status != RATIONAL_OK) return status;

    status = rational_gcd(x.denominator, y.denominator, &g);
    if (status != RATIONAL_OK) return status;
    xd = x.denominator / g;
    yd = y.denominator / g;

    if (checked_mul(x.numerator, yd, &t1)) return RATIONAL_OVERFLOW;
    if (checked_mul(y.numerator, xd, &t2)) return RATIONAL_OVERFLOW;
    if (subtract) {
        if (checked_sub(t1, t2, &raw.numerator)) return RATIONAL_OVERFLOW;
    } else {
        if (checked_add(t1, t2, &raw.numerator)) return RATIONAL_OVERFLOW;
    }
    if (checked_mul(x.denominator, yd, &raw.denominator)) return RATIONAL_OVERFLOW;

    return rational_normalize(raw, result);
}

RationalStatus rational_add(RationalOperands operands, Rational *result)
{
    return add_or_subtract(operands, 0, result);
}

RationalStatus rational_subtract(RationalOperands operands, Rational *result)
{
    return add_or_subtract(operands, 1, result);
}

RationalStatus rational_multiply(RationalOperands operands, Rational *result)
{
    Rational x, y, raw;
    RationalStatus status;

    status = rational_normalize(operands.left, &x);
    if (status != RATIONAL_OK) return status;
    status = rational_normalize(operands.right, &y);
    if (status != RATIONAL_OK) return status;

    if (checked_mul(x.numerator, y.numerator, &raw.numerator)) return RATIONAL_OVERFLOW;
    if (checked_mul(x.denominator, y.denominator, &raw.denominator)) return RATIONAL_OVERFLOW;
    return rational_normalize(raw, result);
}

RationalStatus rational_divide(RationalOperands operands, Rational *result)
{
    Rational x, y, raw;
    RationalStatus status;

    status = rational_normalize(operands.left, &x);
    if (status != RATIONAL_OK) return status;
    status = rational_normalize(operands.right, &y);
    if (status != RATIONAL_OK) return status;

    if (y.numerator == 0) return RATIONAL_DIVISION_BY_ZERO;

    if (checked_mul(x.numerator, y.denominator, &raw.numerator)) return RATIONAL_OVERFLOW;
    if (checked_mul(x.denominator, y.numerator, &raw.denominator)) return RATIONAL_OVERFLOW;
    return rational_normalize(raw, result);
}

RationalStatus rational_compare(RationalOperands operands, int *result)
{
    Rational x, y;
    long long p, q;
    RationalStatus status;

    status = rational_normalize(operands.left, &x);
    if (status != RATIONAL_OK) return status;
    status = rational_normalize(operands.right, &y);
    if (status != RATIONAL_OK) return status;

    if (checked_mul(x.numerator, y.denominator, &p)) return RATIONAL_OVERFLOW;
    if (checked_mul(y.numerator, x.denominator, &q)) return RATIONAL_OVERFLOW;

    *result = (p < q) ? -1 : (p > q) ? 1 : 0;
    return RATIONAL_OK;
}

const char *rational_status_message(RationalStatus status)
{
    switch (status) {
        case RATIONAL_OK: return "Операция выполнена успешно";
        case RATIONAL_ZERO_DENOMINATOR: return "Нулевой знаменатель";
        case RATIONAL_DIVISION_BY_ZERO: return "Деление на ноль";
        case RATIONAL_OVERFLOW: return "Переполнение";
        default: return "Неизвестное состояние";
    }
}