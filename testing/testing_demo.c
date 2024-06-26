#define TESTING_FILE_NAME_ONLY
#define TESTING_SUCCESS_STREAM stdout
#define TESTING_FAILURE_STREAM stderr

#include "testing.h"

#include <string.h>

Testing_Fact(one_plus_one_equals_two) {
    // arrange
    int a = 1, b = 1;

    // act
    int c = a + b;

    // assert
    Testing_Assert(c == 2, "math does not work");
}

Testing_Fact(always_false) {
    int a = 1, b = 2;

    Testing_Assert(a == a, "expected %s=%d to be equal %s=%d", Testing_Nameof(a), a, Testing_Nameof(a), a);
    Testing_Assert(a == b, "expected %s=%d to be equal %s=%d", Testing_Nameof(a), a, Testing_Nameof(b), b);
}

Testing_Fact(strlen_returns_correct_length) {
    const char *s = "abc";
    const size_t expected = 3;

    const size_t length = strlen(s);

    Testing_Assert(length == expected, "expected length to be %zu but was %zu", expected, length);
}

Testing_AllTests = {
        Testing_AddTest(one_plus_one_equals_two),
        Testing_AddTest(always_false),
        Testing_AddTest(strlen_returns_correct_length),
};

Testing_RunAllTests();
