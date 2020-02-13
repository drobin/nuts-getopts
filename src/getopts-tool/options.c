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

#include "internal.h"

static int option_add(struct nuts_getopts_cmdlet_options* options, const char* lname, char sname, int /*nuts_getopts_argument_type*/ arg) {
  const int n = options->nopts + 2;
  struct nuts_getopts_option* opts = realloc(options->opts, n * sizeof(struct nuts_getopts_option));
  char* opts_lname = (lname != NULL) ? malloc(strlen(lname) + 1) : NULL;

  if ((opts == NULL) || ((lname != NULL) && (opts_lname == NULL))) {
    free(opts);
    free(opts_lname);
    return -1;
  }

  opts[n - 2].sname = sname;
  opts[n - 2].lname = (lname != NULL) ? strcpy(opts_lname, lname) : NULL;
  opts[n - 2].arg = arg ? nuts_getopts_required_argument : nuts_getopts_no_argument;
  memset(&opts[n - 1], 0, sizeof(struct nuts_getopts_option));

  options->opts = opts;

  return 0;
}

static int descr_add(struct nuts_getopts_cmdlet_options* options, const char* descr) {
  const int n = options->nopts + 1;
  char** new_arr = realloc(options->descr, n * sizeof(char**));
  char* new_descr = (descr != NULL) ? malloc(strlen(descr) + 1) : NULL;

  if ((new_arr == NULL) || ((descr != NULL) && (new_descr == NULL))) {
    free(new_arr);
    free(new_descr);
    return -1;
  }

  new_arr[options->nopts] = (descr != NULL) ? strcpy(new_descr, descr) : NULL;
  options->descr = new_arr;

  return 0;
}

int nuts_getopts_cmdlet_options_add(struct nuts_getopts_cmdlet_options* options, const char* lname, char sname, int arg, const char* descr) {
  if (options == NULL || (lname == NULL && sname == 0))
    return -1;

  int rc = option_add(options, lname, sname, arg);
  rc = (rc == 0) ? descr_add(options, descr) : rc;

  if (rc == 0)
    options->nopts++;

  return rc;
}

void nuts_getopts_cmdlet_options_release(struct nuts_getopts_cmdlet_options* options) {
  if (options == NULL)
    return;

  for (int i = 0; i < options->nopts; i++) {
    free(options->opts[i].lname);
    free(options->descr[i]);
  }

  free(options->opts);
  free(options->descr);
}
