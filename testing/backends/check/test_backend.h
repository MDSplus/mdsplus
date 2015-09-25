#ifndef CHECK_BACKEND_H
#define CHECK_BACKEND_H



void __test_setfork(int value);

void __test_init(const char *test_name, const char *file, const int line);

void __test_end();

int  __setup_parent();

int  __setup_child();

void __test_assert_fail(const char *file, int line, const char *expr, ...);

void __mark_point(const char *__assertion, const char *__file, unsigned int __line, const char *__function);

void __test_exit() __attribute__ ((__noreturn__));



#endif // CHECK_BACKEND_H

