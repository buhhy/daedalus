#pragma once

#include <stdio.h>

#define TEXT(value) value
#define UE_LOG(t1, t2, ...) printf(__VA_ARGS__);
