#include "arrays.h"

#include <stdint.h>
#include <stdlib.h>

void *allocate(size_t bytes) {
    void *memory = malloc(bytes);
    if (memory == NULL) error("Не удалось выделить память.\n");
    return memory;
}

void arrays_create(Arrays *arrays, size_t count, size_t length) {
    if (count > SIZE_MAX / sizeof(int) / length ||
        length > SIZE_MAX / sizeof(long long)) {
        error("Слишком большой размер массивов.\n");
    }
    arrays->count = count;
    arrays->length = length;
    arrays->data = allocate(count * length * sizeof(int));
}

void arrays_free(Arrays *arrays) {
    free(arrays->data);
}

void arrays_fill(Arrays *arrays) {
    for (size_t i = 0; i < arrays->count * arrays->length; i++) {
        arrays->data[i] = (int)(i % 101) - 50;
    }
}

void sum_part(const Arrays *arrays, long long *result,
              size_t first_array, size_t last_array, size_t begin, size_t end) {
    for (size_t j = begin; j < end; j++) result[j] = 0;
    for (size_t i = first_array; i < last_array; i++) {
        for (size_t j = begin; j < end; j++) {
            result[j] += arrays->data[i * arrays->length + j];
        }
    }
}

void sum_sequential(const Arrays *arrays, long long *result) {
    sum_part(arrays, result, 0, arrays->count, 0, arrays->length);
}

int results_equal(const long long *a, const long long *b, size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}
