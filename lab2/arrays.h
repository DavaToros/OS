#ifndef ARRAYS_H
#define ARRAYS_H

#include <stddef.h>

typedef struct {
    size_t count;
    size_t length;
    int *data;
} Arrays;

void error(const char *text);
void *allocate(size_t bytes);
void arrays_create(Arrays *arrays, size_t count, size_t length);
void arrays_free(Arrays *arrays);
void arrays_fill(Arrays *arrays);
void sum_part(const Arrays *arrays, long long *result,
              size_t first_array, size_t last_array, size_t begin, size_t end);
void sum_sequential(const Arrays *arrays, long long *result);
int choose_threads(const Arrays *arrays, int limit);
void sum_parallel(const Arrays *arrays, long long *result, int threads, int inspect);
int results_equal(const long long *a, const long long *b, size_t length);

#endif
