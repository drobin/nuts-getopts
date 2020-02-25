/******************************************************************************
 * MIT License
 *
 * Copyright (c) 2020 Robin Doer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

#include <ctype.h>
#include <inttypes.h>
#include <stdlib.h>
#include <strings.h>

#include "convert.h"

static const char* skip_ws(const char* str) {
  const char* p = str;

  while (isspace(*p) && *p != '\0')
    p++;

  return p;
}

static void noop_free(void* value) {
  // noop
}

static void general_free(void* value) {
  free(value);
}

static void* str_convert(const char* str, int* ok) {
  *ok = 1;
  return (void*)str;
}

static void* int_convert(const char* str, int* ok) {
  const char* p = skip_ws(str);
  long* result = NULL;

  if (*p == '\0')
    goto err;

  if ((result = malloc(sizeof(long))) == NULL)
    goto err;

  char* endptr = NULL;
  *result = strtol(p, &endptr, 10);

  if (*endptr != '\0')
    goto err;

  *ok = 1;
  return result;

err:

  free(result);
  *ok = 0;
  return NULL;
}

static void* size_convert(const char* str, int* ok) {
  const char* p = skip_ws(str);
  long* result = NULL;

  if (*p == '\0')
    goto err;

  if ((result = malloc(sizeof(long))) == NULL)
    goto err;

  char* endptr = NULL;
  *result = strtol(p, &endptr, 10);

  if (strcasecmp(endptr, "kb") == 0)
    *result *= 1024;
  else if (strcasecmp(endptr, "mb") == 0)
    *result *= (1024 * 1024);
  else if (strcasecmp(endptr, "gb") == 0)
    *result *= (1024 * 1024 * 1024);
  else if (*endptr != '\0')
    goto err;

  *ok = 1;
  return result;

err:

  free(result);
  *ok = 0;
  return NULL;
}

static const struct nuts_getopts_converter str_converter = { .convert = str_convert,   .free = noop_free };
static const struct nuts_getopts_converter int_converter = { .convert = int_convert,   .free = general_free };
static const struct nuts_getopts_converter size_converter = { .convert = size_convert, .free = general_free };

const struct nuts_getopts_converter* nuts_getopts_string_converter() {
  return &str_converter;
}

const struct nuts_getopts_converter* nuts_getopts_int_converter() {
  return &int_converter;
}

const struct nuts_getopts_converter* nuts_getopts_size_converter() {
  return &size_converter;
}
