#pragma once

#include <stdio.h>

// This file mocks out all the Unreal Engine macros.

#define TEXT(value) value
#define UE_LOG(t1, t2, ...) printf(__VA_ARGS__);
