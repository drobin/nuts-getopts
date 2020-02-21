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

#include <stdlib.h>
#include <string.h>

#include "option.h"

void nuts_getopts_cmdlet_option_init(nuts_getopts_cmdlet_option* option) {
  if (option != NULL)
    memset(option, 0, sizeof(struct nuts_getopts_cmdlet_option_s));
}

void nuts_getopts_cmdlet_option_release(nuts_getopts_cmdlet_option* option) {
  if (option != NULL)
    free(option->descr);
}

int nuts_getopts_cmdlet_option_set_descr(nuts_getopts_cmdlet_option* option, const char* descr) {
  if (option == NULL)
    return -1;

  free(option->descr);
  option->descr = NULL;

  if (descr != NULL) {
    if ((option->descr = malloc(strlen(descr) + 1)) != NULL)
      strncpy(option->descr, descr, strlen(descr) + 1);
  }

  return (descr == NULL || option->descr != NULL) ? 0 : -1;
}
