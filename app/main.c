#include <stdio.h>
#include "rational.h"

static int clear_input(void)
{
    int c;
    while ((c = getchar()) != '\n') {
        if (c == EOF) return 0;
    }
    return 1;
}

static int read_rational(const char *name, Rational *value)
{
    char line[100];
    long long n, d;
    int pos = 0;
    RationalStatus status;

    printf("Число %s. Введите числитель и знаменатель через пробел: ", name);
    if (fgets(line, sizeof(line), stdin) == NULL) {
        return 0;
    }

    if (sscanf(line, "%lld %lld %n", &n, &d, &pos) != 2 || line[pos] != '\0') {
        printf("Ошибка: нужно ввести ровно два целых числа.\n");
        return 0;
    }

    status = rational_create(n, d, value);
    if (status != RATIONAL_OK) {
        printf("Ошибка: %s.\n", rational_status_message(status));
        return 0;
    }
    return 1;
}

int main(void)
{
    int choice;

    for (;;) {
        RationalOperands operands;
        Rational result;
        RationalStatus status;
        int cmp;

        printf("\n1 - сложение\n2 - вычитание\n3 - умножение\n");
        printf("4 - деление\n5 - сравнение\n0 - выход\nВыбор: ");

        if (scanf("%d", &choice) != 1) {
            printf("Ошибка: введите номер пункта меню.\n");
            if (!clear_input()) break;
            continue;
        }
        clear_input();

        if (choice == 0) break;
        if (choice < 1 || choice > 5) {
            printf("Нет такого пункта меню.\n");
            continue;
        }

        if (!read_rational("1", &operands.left)) continue;
        if (!read_rational("2", &operands.right)) continue;

        switch (choice) {
            case 1: status = rational_add(operands, &result); break;
            case 2: status = rational_subtract(operands, &result); break;
            case 3: status = rational_multiply(operands, &result); break;
            case 4: status = rational_divide(operands, &result); break;
            default:
                status = rational_compare(operands, &cmp);
                if (status == RATIONAL_OK) {
                    if (cmp < 0) printf("Первое число меньше второго.\n");
                    else if (cmp > 0) printf("Первое число больше второго.\n");
                    else printf("Числа равны.\n");
                    continue;
                }
                break;
        }

        if (status == RATIONAL_OK)
            printf("Результат: %lld/%lld\n", result.numerator, result.denominator);
        else
            printf("Ошибка: %s.\n", rational_status_message(status));
    }
    return 0;
}