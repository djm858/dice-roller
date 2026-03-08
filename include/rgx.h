#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <regex.h>

#define RGX_MAX_LENGTH 20
#define RGX_MAX_MATCHES 10

bool rgx_match();
int rgx_extract();
