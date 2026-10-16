#define _POSIX_C_SOURCE 200809L
#include "arrays.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
    const Arrays *arrays;
    long long *result;
    size_t begin, end;
    int split_arrays;
    int inspect;
} ThreadData;

void *worker(void *argument) {
    ThreadData *data = argument;
    if (data->inspect) sleep(6);
    if (data->split_arrays) {
        sum_part(data->arrays, data->result, data->begin, data->end,
                 0, data->arrays->length);
    } else {
        sum_part(data->arrays, data->result, 0, data->arrays->count,
                 data->begin, data->end);
    }
    return NULL;
}

int choose_threads(const Arrays *arrays, int limit) {
    size_t useful = arrays->count * arrays->length / 10000;
    if (useful == 0) useful = 1;
    if (useful < (size_t)limit) return (int)useful;
    return limit;
}

void sum_parallel(const Arrays *arrays, long long *result, int threads, int inspect) {
    if (threads == 1 && !inspect) {
        sum_sequential(arrays, result);
        return;
    }
    size_t length = arrays->length;
    int split_arrays = length / threads < 16 &&
                       arrays->count >= (size_t)threads;
    size_t work_size = length;
    long long *partial = NULL;
    if (split_arrays) {
        work_size = arrays->count;
        partial = allocate(threads * length * sizeof(long long));
    }
    pthread_t ids[threads];
    ThreadData data[threads];

    for (int i = 0; i < threads; i++) {
        data[i].arrays = arrays;
        data[i].result = result;
        if (split_arrays) data[i].result = partial + i * length;
        data[i].begin = work_size * i / threads;
        data[i].end = work_size * (i + 1) / threads;
        data[i].split_arrays = split_arrays;
        data[i].inspect = inspect;
        if (i > 0 && pthread_create(&ids[i], NULL, worker, &data[i]) != 0) {
            error("Не удалось создать потоки. Уменьшите -t.\n");
        }
    }
    if (inspect) {
        printf("Потоки созданы. Пауза 6 секунд для просмотра в ОС.\n");
        fflush(stdout);
    }
    worker(&data[0]);
    for (int i = 1; i < threads; i++) {
        if (pthread_join(ids[i], NULL) != 0) error("Ошибка ожидания потока.\n");
    }
    if (split_arrays) {
        for (size_t j = 0; j < length; j++) {
            result[j] = 0;
            for (int i = 0; i < threads; i++) result[j] += partial[i * length + j];
        }
        free(partial);
    }
}
