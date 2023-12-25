#include "vector.h"


typedef struct {
    size_t ElementSizeBytes;
    size_t Size;
    size_t Capacity;
} ArrayHeader;

#define IMPL_ARRAY_HEADER(Array) (((ArrayHeader *) Array) - 1)

#define IMPL_ARRAY_GROW_FACTOR 2

void *Array_New(const size_t size, const size_t element_size_bytes) {
    size_t capacity = size > ARRAY_MIN_CAPACITY ? size : ARRAY_MIN_CAPACITY;

    const size_t total_size_bytes = capacity * element_size_bytes + sizeof(ArrayHeader);

    ArrayHeader *header;
    if (NULL == (header = (ArrayHeader *) malloc(total_size_bytes))) {
        perror(nameof_identifier(Array_New));
        return NULL;
    }

    *header = (ArrayHeader) {
            .ElementSizeBytes = element_size_bytes,
            .Size = size,
            .Capacity = capacity,
    };

    return advance_bytes(header, sizeof(ArrayHeader));
}


void Array_Free(void *array, ArrayElementDestructor free_element) {
    if (NULL == array) {
        LOG_NULL(array);
        return;
    }

    if (NULL != free_element) {
        Array_ForEachElement(array, free_element);
    }

    free((void *) IMPL_ARRAY_HEADER(array));
}


size_t Array_ElementSize(const void *array) {
    if (NULL == array) {
        LOG_NULL(array);
        return 0;
    }

    return IMPL_ARRAY_HEADER(array)->ElementSizeBytes;
}


size_t Array_Size(const void *array) {
    if (NULL == array) {
        LOG_NULL(array);
        return 0;
    }

    return IMPL_ARRAY_HEADER(array)->Size;
}


bool Array_IsEmpty(const void *array) {
    return 0 == Array_Size(array);
}


size_t Array_Capacity(const void *array) {
    if (NULL == array) {
        LOG_NULL(array);
        return 0;
    }

    return IMPL_ARRAY_HEADER(array)->Capacity;
}


void *Array_ReserveToFit(void *array, size_t size) {
    if (NULL == array) {
        LOG_NULL(array);
        return array;
    }

    if (Array_Capacity(array) >= size) {
        return array;
    }

    size_t new_capacity = Array_Capacity(array);
    while (new_capacity < size) {
        new_capacity *= IMPL_ARRAY_GROW_FACTOR;
    }

    if (new_capacity < size) {
        LOG_ERROR(
                "new_capacity=%d is not enough for size=%d elements",
                (int) new_capacity, (int) size
        );
        return NULL;
    }
    assert(new_capacity >= size);

    LOG_DEBUG("new_capacity=%d", (int) new_capacity);

    void *new_array;
    if (NULL == (new_array = Array_New(new_capacity, Array_ElementSize(array)))) {
        perror(nameof_identifier(Array_ReserveToFit));
        return NULL;
    }

    memcpy(new_array, array, Array_Size(array) * Array_ElementSize(array));
    IMPL_ARRAY_HEADER(new_array)->Size = Array_Size(array);

    Array_Free(array, NULL);

    return new_array;
}


void *Array_WithAppended(void *array, const void *element) {
    if (NULL == array) {
        LOG_NULL(array);
        return array;
    }

    const size_t old_size = Array_Size(array);
    const size_t old_capacity = Array_Capacity(array);

    const size_t new_size = old_size + 1;

    if (new_size > old_capacity) {
        if (NULL == (array = Array_ReserveToFit(array, new_size))) {
            LOG_ERROR("Failed to increase array capacity");
            return NULL;
        }
    }

    IMPL_ARRAY_HEADER(array)->Size = new_size;
    memcpy(Array_At(array, old_size), element, Array_ElementSize(array));

    LOG_DEBUG(
            "old_size=%d, new_size=%d, old_capacity=%d, new_capacity=%d",
            (int) old_size, (int) new_size, (int) old_capacity, (int) Array_Capacity(array)
    );

    return array;
}


void *Array_ExtendWithValues(
        void *array,
        const void *data,
        const size_t elements_count,
        const size_t element_size
) {
    if (Array_ElementSize(array) != element_size) {
        LOG_ERROR(
                "element_size=%d is different from array element size (%d)",
                (int) element_size, (int) Array_ElementSize(array)
        );
        return NULL;
    }

    const size_t original_size = Array_Size(array);
    const size_t new_size = original_size + elements_count;
    if (NULL == (array = Array_ReserveToFit(array, new_size))) {
        LOG_ERROR("Failed to increase capacity");
        return NULL;
    }

    IMPL_ARRAY_HEADER(array)->Size = new_size;
    for (size_t i = 0; i < elements_count; i++) {
        memcpy(
                Array_At(array, original_size + i),
                advance_bytes(data, i * element_size),
                Array_ElementSize(array)
        );
    }

    assert(Array_Size(array) == original_size + elements_count);

    LOG_DEBUG("Extended array with %d values", (int) elements_count);

    return array;
}


void *Array_Extend(void *array, const void *other) {
    return Array_ExtendWithValues(array, other, Array_Size(other), Array_ElementSize(other));
}


void *Array_OfValues(
        const void *data,
        const size_t elements_count,
        const size_t element_size
) {
    void *array = Array_New(elements_count, element_size);
    IMPL_ARRAY_HEADER(array)->Size = 0;  // ArrayExtendWithValues will work properly, space already reserved
    return Array_ExtendWithValues(array, data, elements_count, element_size);
}


bool Array_PopTo(void *array, void *dst) {
    if (NULL == array) {
        LOG_NULL(array);
        return false;
    }

    if (Array_IsEmpty(array)) {
        LOG_ERROR("Array is empty");
        return false;
    }

    IMPL_ARRAY_HEADER(array)->Size -= 1;

    if (NULL == dst) {
        return true;
    }

    const void *src = Array_At(array, Array_Size(array));
    memcpy(dst, src, Array_ElementSize(array));

    return true;
}