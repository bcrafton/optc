
#ifndef COMMON_H
#define COMMON_H

#include "defines.h"
#include "common_stl.h"

/**
 * Provides the log based 2 ceiling value of a number
 */

int log2number(int num);

/**
 * returns the binary string value of a number
 */

std::string num2binary(int count, int width);

/**
 * Splits a string based on the delimmiter.
 */

vector_t<std::string> split(const std::string& str, const std::string& delim);

#endif
