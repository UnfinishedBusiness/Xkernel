#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#ifndef WINDOWS_SPECIFIC_H
#define WINDOWS_SPECIFIC_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <cstddef>

size_t getline(char **lineptr, size_t *n, FILE *stream);

#endif
#endif