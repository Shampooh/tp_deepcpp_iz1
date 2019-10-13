/*
 * Создать структуру для хранения информации о дорожной сети:
 * - протяженности дорог,
 * - виде дорожного полотна,
 * - качества покрытия
 * - и количества полос движения транспорта.
 * Составить с ее использованием программу определения среднего качества дорог
 * с заданным видом полотна
 * и заданным числом полос для движения.
 */

// https://stackoverflow.com/questions/3437404/min-and-max-in-c
// https://stackoverflow.com/questions/17052443/c-function-inside-struct
// https://stackoverflow.com/questions/17621544/dynamic-method-dispatching-in-c
// https://github.com/LouisBrunner/valgrind-macos
// https://www.jetbrains.com/help/clion/creating-google-test-run-debug-configuration-for-test.html

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define max(a, b) \
   ({ typeof (a) _a = (a); \
       typeof (b) _b = (b); \
     _a > _b ? _a : _b; })

//Определяем новые типы
typedef struct road_quality RoadQuality;
typedef struct test_road_quality TestRoadQuality;
typedef struct road_container RoadContainer;
typedef struct char_container CharContainer;
typedef struct dynamic_road_array_interface DynamicRoadArrayInterface;
typedef struct dynamic_char_array_interface DynamicCharArrayInterface;
typedef struct dynamic_road_array DynamicRoadArray;
typedef struct dynamic_char_array DynamicCharArray;

//Определяем новые структуры
struct road_quality {
    int id;
    int length;
    int road_type;
    int road_qual;
    int line_count;
};

struct test_road_quality {
    int road_type;
    int line_count;
};

struct dynamic_road_array {
    const DynamicRoadArrayInterface *const vtable;
};

struct dynamic_char_array {
    const DynamicCharArrayInterface *const vtable;
};

struct dynamic_road_array_interface {
    void (*init_road)();

    const int (*size_road)(DynamicRoadArray *);

    const RoadQuality (*get_road)(DynamicRoadArray *, int);

    void (*add_road)(DynamicRoadArray *, RoadQuality);

    void (*grow_road)(DynamicRoadArray *);

    void (*destroy_road)(DynamicRoadArray *);
};

struct dynamic_char_array_interface {
    void (*init_char)();

    const int (*size_char)(DynamicCharArray *);

    const char (*get_char)(DynamicCharArray *, int);

    const char *(*get_full_char)(DynamicCharArray *);

    void (*add_char)(DynamicCharArray *, char[]);

    void (*grow_char)(DynamicCharArray *);

    void (*destroy_char)(DynamicCharArray *);
};

struct road_container {
    DynamicRoadArray base;
    RoadQuality *buffer;
    int bufferSize;
    int realSize;
    int defaultInitialSize;
};

struct char_container {
    DynamicCharArray base;
    char *buffer;
    int bufferSize;
    int realSize;
    int defaultInitialSize;
};

extern inline void darray_init_road(DynamicRoadArray *s) { s->vtable->init_road(s); }

extern inline const int darray_size_road(DynamicRoadArray *s) { return s->vtable->size_road(s); }

extern inline const RoadQuality darray_get_road(DynamicRoadArray *s, int index) {
    return s->vtable->get_road(s, index);
}

extern inline void darray_add_road(DynamicRoadArray *s, RoadQuality r) { s->vtable->add_road(s, r); }

extern inline void darray_grow_road(DynamicRoadArray *s) { s->vtable->grow_road(s); }

extern inline void darray_destroy_road(DynamicRoadArray *s) { s->vtable->destroy_road(s); }

extern inline void darray_init_char(DynamicCharArray *s) { s->vtable->init_char(s); }

extern inline const int darray_size_char(DynamicCharArray *s) { return s->vtable->size_char(s); }

extern inline const char darray_get_char(DynamicCharArray *s, int index) { return s->vtable->get_char(s, index); }

extern inline const char *darray_get_full_char(DynamicCharArray *s) { return s->vtable->get_full_char(s); }

extern inline void darray_add_char(DynamicCharArray *s, char c) { s->vtable->add_char(s, c); }

extern inline void darray_grow_char(DynamicCharArray *s) { s->vtable->grow_char(s); }

extern inline void darray_destroy_char(DynamicCharArray *s) { s->vtable->destroy_char(s); }

static void dynamic_init_road(DynamicRoadArray *s) {
    RoadContainer *cont = (void *) s;
    cont->buffer = 0;
    cont->realSize = 0;
    cont->bufferSize = 0;
    cont->defaultInitialSize = 2;
}

static const int dynamic_size_road(DynamicRoadArray *s) {
    RoadContainer *cont = (void *) s;
    return cont->realSize;
}

static const RoadQuality dynamic_get_road(DynamicRoadArray *s, int index) {
    RoadContainer *cont = (void *) s;
    return cont->buffer[index];
}

static void dynamic_add_road(DynamicRoadArray *s, RoadQuality r) {
    RoadContainer *cont = (void *) s;

    if (cont->realSize == cont->bufferSize) {
        darray_grow_road(s);
    }

    cont->buffer[cont->realSize++] = r;
}

static void dynamic_grow_road(DynamicRoadArray *s) {
    RoadContainer *cont = (void *) s;
    int newBufferSize = max(cont->bufferSize * 2, cont->defaultInitialSize);
    RoadQuality *newBuffer = (RoadQuality *) malloc(newBufferSize * sizeof(RoadQuality));;

    for (int i = 0; i < cont->realSize; ++i) {
        newBuffer[i] = cont->buffer[i];
    }

    free(cont->buffer);
    cont->buffer = 0;

    cont->buffer = newBuffer;
    cont->bufferSize = newBufferSize;
}

static void dynamic_destroy_road(DynamicRoadArray *s) {
    RoadContainer *cont = (void *) s;
    free(cont->buffer);
    cont->buffer = 0;
    cont->realSize = 0;
    cont->bufferSize = 0;
    cont->defaultInitialSize = 2;
}

DynamicRoadArray *road_darray_create() {

    static const DynamicRoadArrayInterface vtable = {
            dynamic_init_road, dynamic_size_road, dynamic_get_road,
            dynamic_add_road, dynamic_grow_road, dynamic_destroy_road
    };
    static DynamicRoadArray base = {&vtable};
    RoadContainer *cont = malloc(sizeof(*cont));
    memcpy(&cont->base, &base, sizeof(base));
    darray_init_road(cont);
    return &cont->base;
}

static void dynamic_init_char(DynamicCharArray *s) {
    CharContainer *cont = (void *) s;
    cont->buffer = 0;
    cont->realSize = 0;
    cont->bufferSize = 0;
    cont->defaultInitialSize = 2;
}

static const int dynamic_size_char(DynamicCharArray *s) {
    CharContainer *cont = (void *) s;
    return cont->realSize;
}

static const char dynamic_get_char(DynamicCharArray *s, int index) {
    CharContainer *cont = (void *) s;
    return cont->buffer[index];
}

static const char *dynamic_get_full_char(DynamicCharArray *s) {
    CharContainer *cont = (void *) s;
    const int char_size = darray_size_char(s);
    if (char_size <= 255) {
        char *res = (char *) malloc(sizeof(char) * char_size);
        for (int i = 0; i < char_size; i++) {
            res[i] = darray_get_char(s, i);
        }
        return res;
    } else { return 0; }
}

static void dynamic_add_char(DynamicCharArray *s, char c) {
    CharContainer *cont = (void *) s;

    if (cont->realSize == cont->bufferSize) {
        darray_grow_char(s);
    }

    cont->buffer[cont->realSize++] = c;
}

static void dynamic_grow_char(DynamicCharArray *s) {
    CharContainer *cont = (void *) s;
    int newBufferSize = max(cont->bufferSize * 2, cont->defaultInitialSize);
    char *newBuffer = (char *) malloc(newBufferSize * sizeof(char));;

    for (int i = 0; i < cont->realSize; ++i) {
        newBuffer[i] = cont->buffer[i];
    }

    free(cont->buffer);
    cont->buffer = 0;
    cont->buffer = newBuffer;
    cont->bufferSize = newBufferSize;
}

static void dynamic_destroy_char(DynamicCharArray *s) {
    CharContainer *cont = (void *) s;
    free(cont->buffer);
    cont->buffer = 0;
    cont->realSize = 0;
    cont->bufferSize = 0;
    cont->defaultInitialSize = 2;
}

DynamicCharArray *char_darray_create() {
    static const DynamicCharArrayInterface vtable = {
            dynamic_init_char, dynamic_size_char, dynamic_get_char,
            dynamic_get_full_char, dynamic_add_char, dynamic_grow_char,
            dynamic_destroy_char
    };
    static DynamicCharArray base = {&vtable};
    CharContainer *cont = malloc(sizeof(*cont));
    memcpy(&cont->base, &base, sizeof(base));
    darray_init_char(cont);
    return &cont->base;
}

bool validation(RoadQuality input) {
    return (input.id > 0) && (input.length > 0) && (input.road_type > 0) && (input.line_count > 0);
}

bool test_validation(TestRoadQuality input) {
    return (input.road_type > 0) && (input.line_count > 0);
}

RoadQuality get_default_input() {
    RoadQuality input = {.id = -1, .length = -1, .road_qual = -1, .road_type = -1, .line_count = -1};
    return input;
}

TestRoadQuality get_test_input() {
    TestRoadQuality input = {.road_type = -1, .line_count = -1};
    return input;
}

void add_input_data(DynamicCharArray *da_char, RoadQuality *input, int number) {
    const char *full_char = darray_get_full_char(da_char);
    const int c_size = darray_size_char(da_char);
    const char road_elm[c_size];
    strncpy(road_elm, full_char, c_size);

    switch (number) {
        case 0:
            input->id = atoi(road_elm);
            break;
        case 1:
            input->length = atoi(road_elm);
            break;
        case 2:
            input->road_type = atoi(road_elm);
            break;
        case 3:
            input->road_qual = atoi(road_elm);
            break;
        case 4:
            input->line_count = atoi(road_elm);
            break;
        default:
            break;
    }
    free(full_char);
}

void add_test_data(DynamicCharArray *da_char, TestRoadQuality *input, int number) {
    const char *full_char = darray_get_full_char(da_char);
    const int c_size = darray_size_char(da_char);
    const char road_elm[c_size];
    strncpy(road_elm, full_char, c_size);

    switch (number) {
        case 0:
            input->road_type = atoi(road_elm);
            break;
        case 1:
            input->line_count = atoi(road_elm);
            break;
        default:
            break;
    }
    free(full_char);
}

void *read_input_file(DynamicRoadArray *da_road) {
    RoadQuality input = get_default_input();
    DynamicCharArray *da_char = char_darray_create();
    FILE *file;

    file = fopen("./input.txt", "r");

    if (!file) {
        return 0;
    }

    int i = 0;
    char c = getc(file);
    while (c != EOF) {
        if (c != '\n') {
            if (c != ' ') {
                darray_add_char(da_char, c);
            } else {
                if (i < 4) {
                    add_input_data(da_char, &input, i);
                    darray_destroy_char(da_char);
                }
                i++;
            }
        } else {
            if (i == 4) {
                add_input_data(da_char, &input, i);
                darray_destroy_char(da_char);
                if (validation(input)) {
                    darray_add_road(da_road, input);
                }
            }
            i = 0;
            input = get_default_input();
        }
        c = getc(file);
    }

    fclose(file);
    free(da_char);
}

void *read_test_file(TestRoadQuality *input) {
    DynamicCharArray *da_char = char_darray_create();
    FILE *file;

    file = fopen("./test.txt", "r");

    if (!file) {
        return 0;
    }

    int i = 0;
    char c = getc(file);
    while (c != EOF) {
        if (c != '\n') {
            if (c != ' ') {
                darray_add_char(da_char, c);
            } else {
                if (i < 1) {
                    add_test_data(da_char, input, i);
                    darray_destroy_char(da_char);
                }
                i++;
            }
        } else {
            if (i == 1) {
                add_test_data(da_char, input, i);
                darray_destroy_char(da_char);
                break;
            }
            i = 0;
            input->line_count = -1;
            input->road_type = -1;
        }
        c = getc(file);
    }

    fclose(file);
    free(da_char);
}

int main() {
    DynamicRoadArray *da_road = road_darray_create();

    read_input_file(da_road);

    printf("INPUT FROM A FILE:\n");
    for (int i = 0; i < darray_size_road(da_road); i++) {
        printf("ID: %d LENGTH: %d TYPE: %d QUAL: %d LINES: %d\n",
               darray_get_road(da_road, i).id,
               darray_get_road(da_road, i).length,
               darray_get_road(da_road, i).road_type,
               darray_get_road(da_road, i).road_qual,
               darray_get_road(da_road, i).line_count);
    }

    TestRoadQuality test_input = get_test_input();
    read_test_file( &test_input );

    printf("\nREAD TEST DATA:\n");
    printf("TYPE: %d LINES: %d\n", test_input.road_type, test_input.line_count);
    int k = 0;
    int road_type_sum = 0;

    for (int i = 0; i < darray_size_road( da_road ); i++) {
        if ( darray_get_road( da_road, i ).road_type == test_input.road_type &&
                darray_get_road( da_road, i ).line_count == test_input.line_count ) {
            road_type_sum += darray_get_road( da_road, i ).road_qual;
            k++;
        }
    }

    int answer = 0;//road_type_sum / k;

    printf("\nANSWER: %d \n", answer);

    darray_destroy_road(da_road);
    free(da_road);
    return 0;
}