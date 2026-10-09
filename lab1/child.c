#include <limits.h>
#include <unistd.h>

static int print_sum(long long sum)
{
    char buffer[32];
    size_t position = sizeof(buffer) - 1;
    int negative = sum < 0;
    buffer[position] = '\n';

    do {
        int digit = sum % 10;
        if (digit < 0) {
            digit = -digit;
        }
        buffer[--position] = '0' + digit;
        sum /= 10;
    } while (sum != 0);
    if (negative) {
        buffer[--position] = '-';
    }

    for (; position < sizeof(buffer); ++position) {
        if (write(STDOUT_FILENO, &buffer[position], 1) != 1) {
            return 1;
        }
    }
    return 0;
}

int main(void)
{
    long long number = 0, sum = 0;
    int sign = 1, has_number = 0, has_line = 0;
    char c;

    for (;;) {
        ssize_t count = read(STDIN_FILENO, &c, 1);
        if (count < 0) {
            return 1;
        }

        if (count == 0) {
            c = '\n';
        }
        if (c >= '0' && c <= '9') {
            number = number * 10 + (c - '0');
            if (number > (sign > 0 ? INT_MAX : -(long long)INT_MIN)) {
                return 1;
            }
            has_number = 1;
        } else if (c == '-' || c == '+') {
            if (has_number) {
                return 1;
            }
            sign = c == '-' ? -1 : 1;
        } else if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            if (has_number) {
                sum += sign * number;
                has_line = 1;
            }
            number = 0;
            sign = 1;
            has_number = 0;
            if (c == '\n' && has_line) {
                if (print_sum(sum) != 0) {
                    return 1;
                }
                sum = 0;
                has_line = 0;
            }
        } else {
            return 1;
        }
        if (count == 0) {
            break;
        }
    }
    return 0;
}
