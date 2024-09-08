#pragma once
#include "../exlibs.h"
#include <assert.h>
#include <stdint.h>

#define ASSERT(x) \
    if (!(x))     \
    assert(false)
#define INT2VOIDP(i) (void *)(uintptr_t)(i)

#ifdef DEBUG
#define GLCall(x)   \
    GLClearError(); \
    x;              \
    ASSERT(GLCheckError())
#else
#define GLCall(x) x
#endif
