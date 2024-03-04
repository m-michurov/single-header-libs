#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "call_checked.h"
#include "vector.h"

#include "testing/testing.h"

Testing_Fact(empty_vector_has_size_and_capacity_of_0) {
    Vector_Of(int) sut = {0};

    Testing_Assert(0 == sut.Size, "size must be 0 for empty vector");
    Testing_Assert(0 == sut.Capacity, "capacity must be 0 for empty vector");
}

Testing_Fact(PushBack_appends_elements) {
    Vector_Of(int) sut = {0};

    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    size_t const elementsCount = sizeof(elements) / sizeof(elements[0]);

    for (size_t i = 0; i < elementsCount; i++) {
        Vector_PushBack(&sut, elements[i]);
    }

    Testing_Assert(elementsCount == sut.Size, "expected size to be %zu but was %zu", elementsCount, sut.Size);
    for (size_t i = 0; i < elementsCount; i++) {
        Testing_Assert(
                elements[i] == sut.Items[i],
                "expected element %zu to be %d but was %d", i, elements[i], sut.Items[i]
        );
    }

    Vector_Free(&sut);
}

Testing_Fact(Free_sets_size_and_capacity_to_0) {
    Vector_Of(int) sut = {0};

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);

    Vector_Free(&sut);
    Testing_Assert(0 == sut.Size, "size must be 0 after free");
    Testing_Assert(0 == sut.Capacity, "capacity must be 0 after free");
}

Testing_Fact(TryPopBack_returns_false_for_empty_vector) {
    Vector_Of(int) sut = {0};

    int value;
    Testing_Assert(false == Vector_TryPopBack(&sut, &value), "expected TryPopBack to return false for empty vector");
}

Testing_Fact(TryPopBack_returns_elements_in_reverse_order) {
    Vector_Of(int) sut = {0};

    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    size_t const elementsCount = sizeof(elements) / sizeof(elements[0]);

    for (size_t i = 0; i < elementsCount; i++) {
        Vector_PushBack(&sut, elements[i]);
    }

    size_t j = elementsCount - 1;
    int value;
    while (Vector_TryPopBack(&sut, &value)) {
        Testing_Assert(elements[j] == value, "expected element %zu to be %d but was %d", j, elements[j], value);
        j--;
    }
    Testing_Assert(0 == sut.Size, "expected vector to be empty after TryPopBack returns false");

    Vector_Free(&sut);
}

Testing_Fact(ForEach_never_executes_body_for_empty_list) {
    Vector_Of(int) sut = {0};

    int bodyExecuted = false;
    Vector_ForEach(pValue, sut) {
        (void) pValue;
        bodyExecuted = true;
    }

    Testing_Assert(false == bodyExecuted, "body must not be executed for empty list");

    Vector_Free(&sut);
}

Testing_Fact(ForEach_iterates_over_all_elements) {
    Vector_Of(int) sut = {0};

    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    size_t const elementsCount = sizeof(elements) / sizeof(elements[0]);
    for (size_t i = 0; i < elementsCount; i++) {
        Vector_PushBack(&sut, elements[i]);
    }

    size_t j = 0;
    Vector_ForEach(pValue, sut) {
        Testing_Assert(elements[j] == *pValue, "expected value %zu to be %d but was %d", j, elements[j], *pValue);
        j++;
    }
    Testing_Assert(
            elementsCount == j,
            "expected count of elements iterated over to be %zu but was %zu", elementsCount, j
    );

    Vector_Free(&sut);
}

Testing_Fact(At_returns_pointer_to_element_for_valid_non_negative_index) {
    Vector_Of(int) sut = {0};

    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    size_t const elementsCount = sizeof(elements) / sizeof(elements[0]);
    for (size_t i = 0; i < elementsCount; i++) {
        Vector_PushBack(&sut, elements[i]);
    }

    for (size_t i = 0; i < sut.Size; i++) {
        int const *valuePtr = Vector_At(sut, i);
        Testing_Assert(&sut.Items[i] == valuePtr, "At returned wrong pointer");
    }

    Vector_Free(&sut);
}

Testing_Fact(At_returns_pointer_to_element_for_valid_negative_index) {
    Vector_Of(int) sut = {0};

    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    size_t const elementsCount = sizeof(elements) / sizeof(elements[0]);
    for (size_t i = 0; i < elementsCount; i++) {
        Vector_PushBack(&sut, elements[i]);
    }

    for (int i = 1; i <= (int) sut.Size; i++) {
        int const *valuePtr = Vector_At(sut, -i);
        Testing_Assert(&sut.Items[sut.Size - i] == valuePtr, "At returned wrong pointer");
    }

    Vector_Free(&sut);
}

Testing_Fact(At_returns_NULL_for_invalid_indices) {
    Vector_Of(int) sut = {0};

    int const elements[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    size_t const elementsCount = sizeof(elements) / sizeof(elements[0]);
    for (size_t i = 0; i < elementsCount; i++) {
        Vector_PushBack(&sut, elements[i]);
    }

    Testing_Assert(NULL == Vector_At(sut, sut.Size), "expected At to return NULL");
    Testing_Assert(NULL == Vector_At(sut, -(int) sut.Size - 1), "expected At to return NULL");

    Vector_Free(&sut);
}

Testing_Fact(Empty_returns_true_for_empty_vector) {
    Vector_Of(int) sut = {0};

    Testing_Assert(Vector_Empty(sut), "expected Empty to return true");

    Vector_Free(&sut);
}

Testing_Fact(Empty_returns_false_for_non_empty) {
    Vector_Of(int) sut = {0};

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);

    Testing_Assert(false == Vector_Empty(sut), "expected Empty to return false");

    Vector_Free(&sut);
}

Testing_Fact(Empty_returns_true_for_vector_with_all_elements_removed) {
    Vector_Of(int) sut = {0};

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);

    int value;
    while (Vector_TryPopBack(&sut, &value)) {}

    Testing_Assert(Vector_Empty(sut), "expected Empty to return true");

    Vector_Free(&sut);
}

Testing_Fact(Empty_returns_true_after_Clear) {
    Vector_Of(int) sut = {0};

    Vector_PushBack(&sut, 1);
    Vector_PushBack(&sut, 2);
    Vector_PushBack(&sut, 3);

    Vector_Clear(&sut);

    Testing_Assert(Vector_Empty(sut), "expected Empty to return true");

    Vector_Free(&sut);
}

Testing_AllTests = {
        Testing_AddTest(empty_vector_has_size_and_capacity_of_0),
        Testing_AddTest(PushBack_appends_elements),
        Testing_AddTest(Free_sets_size_and_capacity_to_0),
        Testing_AddTest(TryPopBack_returns_false_for_empty_vector),
        Testing_AddTest(TryPopBack_returns_elements_in_reverse_order),
        Testing_AddTest(ForEach_never_executes_body_for_empty_list),
        Testing_AddTest(ForEach_iterates_over_all_elements),
        Testing_AddTest(At_returns_pointer_to_element_for_valid_non_negative_index),
        Testing_AddTest(At_returns_pointer_to_element_for_valid_negative_index),
        Testing_AddTest(At_returns_NULL_for_invalid_indices),
        Testing_AddTest(Empty_returns_true_for_empty_vector),
        Testing_AddTest(Empty_returns_false_for_non_empty),
        Testing_AddTest(Empty_returns_true_for_vector_with_all_elements_removed),
        Testing_AddTest(Empty_returns_true_after_Clear),
};

Testing_RunAllTests();