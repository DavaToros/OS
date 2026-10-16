#define _POSIX_C_SOURCE 200809L
#include "arrays.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void error(const char *text) {
    fprintf(stderr, "%s", text);
    exit(1);
}

int read_number(const char *text) {
    char *end;
    errno = 0;
    long value = strtol(text, &end, 10);
    if (errno != 0 || *end != '\0' || value <= 0 || value > INT_MAX) {
        error("Ожидалось положительное целое число.\n");
    }
    return (int)value;
}

double current_time_ms(void) {
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec * 1000.0 + time.tv_nsec / 1000000.0;
}

int main(int argc, char *argv[]) {
    if ((argc != 7 && argc != 8) || strcmp(argv[1], "-k") != 0 ||
        strcmp(argv[3], "-n") != 0 || strcmp(argv[5], "-t") != 0 ||
        (argc == 8 && strcmp(argv[7], "-s") != 0)) {
        error("Использование: ./main -k K -n N -t ПОТОКИ [-s]\n");
    }
    int k = read_number(argv[2]);
    int n = read_number(argv[4]);
    int limit = read_number(argv[6]);
    int inspect = argc == 8;
    if (limit > 1024) error("Допустимо не больше 1024 потоков.\n");

    Arrays arrays;
    arrays_create(&arrays, (size_t)k, (size_t)n);
    arrays_fill(&arrays);
    long long *sequential = allocate((size_t)n * sizeof(long long));
    long long *parallel = allocate((size_t)n * sizeof(long long));
    int threads = choose_threads(&arrays, limit);

    // Прогрев памяти перед замерами.
    sum_sequential(&arrays, sequential);
    sum_sequential(&arrays, parallel);
    double start = current_time_ms();
    sum_sequential(&arrays, sequential);
    double sequential_ms = current_time_ms() - start;
    start = current_time_ms();
    sum_parallel(&arrays, parallel, threads, inspect);
    double parallel_ms = current_time_ms() - start;
    int ok = results_equal(sequential, parallel, (size_t)n);

    printf("Массивов: %d\nДлина: %d\n", k, n);
    printf("Лимит потоков: %d\nИспользовано потоков: %d\n", limit, threads);
    printf("Стратегия: %s\n", threads == 1 ? "последовательная" :
           (n / threads < 16 && k >= threads ? "по массивам" : "по элементам"));
    if (!inspect) {
        double speedup = parallel_ms > 0 ? sequential_ms / parallel_ms : 0;
        printf("Последовательно, мс: %.3f\n", sequential_ms);
        printf("Параллельно, мс: %.3f\n", parallel_ms);
        printf("Ускорение: %.3f\n", speedup);
        printf("Эффективность, %%: %.3f\n", speedup / threads * 100);
    }
    printf("Проверка: %s\n", ok ? "OK" : "ОШИБКА");
    if (n <= 20) {
        printf("Сумма:");
        for (int j = 0; j < n; j++) printf(" %lld", parallel[j]);
        printf("\n");
    }
    arrays_free(&arrays);
    free(sequential);
    free(parallel);
    return ok ? 0 : 1;
}
