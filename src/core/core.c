
#include <stdio.h>

/*


    array
        -at
        -size
        -copy

    vector
    

    array
    -iterator()
    -
*/


void log(FILE *fd, const char *__msg, const char *__caller, const char *__file, const int __line) {
    fprintf(fd, "[ERR: %s:%i <%s>]: %s\n", __file, __line, __caller, __msg);
}

#define __log_err(msg) log(stderr, msg, __func__, __FILE__, __LINE__);
#define __log_info(msg) log(stdout, msg, __func__, __FILE__, __LINE__);

// ::::::: Iterators
typedef struct basic_iterator {
    void *begin;
    unsigned int diff;
    size_t len;
    size_t cur;
} basic_iterator;

#define __iter_begin(ITER, TYPE) (TYPE *) iter_begin(ITER);
#define __iter_end(ITER, TYPE) (TYPE *) iter_end(ITER);
#define __iter_cur(ITER, TYPE) (TYPE *) iter_cur(ITER);
#define __iter_set(ITER, POS) iter_set(ITER, POS);
#define __iter_next(ITER, TYPE) (TYPE *) iter_next(ITER);
#define __iter_back(ITER, TYPE) (TYPE *) iter_back(ITER);

void iter_init(basic_iterator *it, void *__begin, unsigned int __diff, size_t __len) {
    it->begin = __begin;
    it->diff = __diff;
    it->len = __len;
    it->cur = 0;
}

void iter_set(basic_iterator *it, size_t __pos) {
    if (__pos >= it->len) {
        __log_err("pos>=it.len");
    }
    it->cur = __pos;
}

void *iter_begin(basic_iterator *it) { return it->begin; }

void *iter_end(basic_iterator *it) { return (void *) it->begin + ((it->len - 1) * it->diff); }

void *iter_cur(basic_iterator *it) { return (void *) it->begin + (it->cur * it->diff); }

void *iter_next(basic_iterator *it) {
    if (it->cur >= it->len) {
        __log_err("cur>it.len");
    } else {
        it->cur++;
        return iter_cur(it);
    }
}

void *iter_back(basic_iterator *it) {
    if (it->cur >= it->len) {
        __log_err("cur>it.len");
    } else {
        it->cur--;
        return iter_cur(it);
    }
}

int foo() {
    __log_err("sample error!");
    return 5;
}

int main() {
    printf("HI!\n");

    int t[4] = {1, 2, 3, 4};

    basic_iterator it;
    iter_init(&it, t, sizeof(int), 4);

    printf("init: begin=%p\n", iter_begin(&it));
    printf("init: end=%p\n", iter_end(&it));
    printf("init: duff=%i\n", it.diff);

    int *cur = __iter_cur(&it, int);
    printf("cur=%i\n", *cur);

    cur = __iter_end(&it, int);
    printf("cur=%i\n", *cur);


    foo();

    return 0;
}