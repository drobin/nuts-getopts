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

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "option.h"

void nuts_getopts_cmdlet_option_init(nuts_getopts_cmdlet_option* option, int conv_id) {
  if (option != NULL) {
    memset(option, 0, sizeof(struct nuts_getopts_cmdlet_option_s));
    option->conv_id = conv_id;
  }
}

void nuts_getopts_cmdlet_option_release(nuts_getopts_cmdlet_option* option) {
  if (option != NULL) {
    free(option->descr);
    free(option->arg);
  }
}

int nuts_getopts_cmdlet_option_set_descr(nuts_getopts_cmdlet_option* option, const char* descr, ...) {
  if (option == NULL)
    return -1;

  free(option->descr);
  option->descr = NULL;

  if (descr != NULL) {
    va_list ap;

    va_start(ap, descr);
    vasprintf(&option->descr, descr, ap);
    va_end(ap);
  }

  return (descr == NULL || option->descr != NULL) ? 0 : -1;
}

int nuts_getopts_cmdlet_option_set_arg(nuts_getopts_cmdlet_option* option, const char* arg) {
  if (option == NULL)
    return -1;

  free(option->arg);
  option->arg = NULL;

  if (arg != NULL) {
    if ((option->arg = malloc(strlen(arg) + 1)) != NULL)
      strncpy(option->arg, arg, strlen(arg) + 1);
  }

  return (arg == NULL || option->arg != NULL) ? 0 : -1;
}
