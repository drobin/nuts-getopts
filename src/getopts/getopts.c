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

#include "nuts-getopts.h"

#define _group_eof(entry) (((entry)->group == NULL) && ((entry)->list == NULL))
#define _list_eof(entry) (((entry)->sname == 0) && ((entry)->lname == NULL))

static inline int is_shortopt(const char* str) {
  return (str[0] == '-') && (str[1] != '\0');
}

static inline int is_longopt(const char* str) {
  return (str[0] == '-') && (str[1] == '-') && (str[2] != '\0');
}

static inline int has_flag(int flags, nuts_getopts_flag flag) {
  return ((flags & flag) > 0);
}

static void mk_option_event(struct nuts_getopts_event* event, const struct nuts_getopts_option* option, const char* value) {
  if (event != NULL) {
    event->type = nuts_getopts_option_event;
    event->u.opt.option = option;
    event->u.opt.value = value;
  }
}

static void mk_error_event(struct nuts_getopts_event* event, nuts_getopts_error_type code, const char* option, int option_len) {
  if (event != NULL) {
    event->type = nuts_getopts_error_event;
    event->u.err.type = code;
    event->u.err.option = option;
    event->u.err.option_len = option_len;
  }
}

static const struct nuts_getopts_option* find_option(const struct nuts_getopts_option_group* options, const char sname, const char* lname, int lname_len) {
  const struct nuts_getopts_option_group* entry = options;

  while (!_group_eof(entry)) {
    if (entry->group != NULL) {
      const struct nuts_getopts_option* option = find_option(entry->group, sname, lname, lname_len);

      if (option != NULL)
        return option;
    }

    if (entry->list != NULL) {
      const struct nuts_getopts_option* option = entry->list;

      while (!_list_eof(option)) {
        if ((sname != 0 && option->sname == sname) ||
            (lname != NULL && option->lname != NULL && strncmp(option->lname, lname, lname_len) == 0))
          return option;
        option++;
      }
    }

    entry++;
  }

  return NULL;
}

static int on_tool(int argc, char* argv[], nuts_getopts_state* state, struct nuts_getopts_event* event) {
  if (event != NULL) {
    const char* arg = argv[state->idx];
    const char* pos = strrchr(arg, '/');

    event->type = nuts_getopts_tool_event;
    event->u.tool =  (pos != NULL) ? pos + 1 : arg;
  }

  state->idx++;

  return 0;
}

static int on_shortopt(int argc, char* argv[], const struct nuts_getopts_option_group* options, int flags, nuts_getopts_state* state, struct nuts_getopts_event* event) {
  const char* option = argv[state->idx];
  const char name = option[1];
  const struct nuts_getopts_option* opt = find_option(options, name, NULL, 0);

  int again = 0;

  if (opt == NULL) {
    if (has_flag(flags, nuts_getopts_ignore_unknown_options))
      again = 1;
    else
      mk_error_event(event, nuts_getopts_invalid_option, option, 2);
  } else if (opt->arg == nuts_getopts_no_argument) {
    if (strlen(option) == 2)
      mk_option_event(event, opt, NULL);
    else
      mk_error_event(event, nuts_getopts_needless_value, option, 2);
  } else {
    if (strlen(option) > 2)
      mk_option_event(event, opt, option + 2);
     else
      mk_error_event(event, nuts_getopts_missing_value, option, 2);
  }

  state->idx++;

  return again;
}

static int on_longopt(int argc, char* argv[], const struct nuts_getopts_option_group* options, int flags, nuts_getopts_state* state, struct nuts_getopts_event* event) {
  const char* option = argv[state->idx];
  const char* name = option + 2;
  const char* eq = strchr(name, '=');
  int name_len = (eq != NULL) ? eq - name : strlen(name);

  const struct nuts_getopts_option* opt = find_option(options, 0, name, name_len);
  int again = 0;

  if (opt == NULL) {
    if (has_flag(flags, nuts_getopts_ignore_unknown_options))
      again = 1;
    else
      mk_error_event(event, nuts_getopts_invalid_option, option, name_len + 2);
  } else if (opt->arg == nuts_getopts_no_argument) {
    if (eq == NULL)
      mk_option_event(event, opt, NULL);
     else
      mk_error_event(event, nuts_getopts_needless_value, option, name_len + 2);
  } else {
    if (eq != NULL)
      mk_option_event(event, opt, eq + 1);
    else
      mk_error_event(event, nuts_getopts_missing_value, option, name_len + 2);
  }

  state->idx++;

  return again;
}

static int on_argument(int argc, char* argv[], nuts_getopts_state* state, struct nuts_getopts_event* event) {
  if (event != NULL) {
    event->type = nuts_getopts_argument_event;
    event->u.arg = argv[state->idx];
  }

  state->idx++;

  return 0;
}

int nuts_getopts(int argc, char* argv[], const struct nuts_getopts_option* options, int flags, nuts_getopts_state* state, struct nuts_getopts_event* event) {
  const struct nuts_getopts_option_group all_options[] = {
    { .group = NULL, .list = options },
    { NULL, NULL }
  };

  return nuts_getopts_group(argc, argv, all_options, flags, state, event);
}

int nuts_getopts_group(int argc, char* argv[], const struct nuts_getopts_option_group* options, int flags, nuts_getopts_state* state, struct nuts_getopts_event* event) {
  memset(event, 0, sizeof(struct nuts_getopts_event));

  int again = 1;

  while (again) {
    if (state->idx >= argc)
      return -1;

    if (state->idx == 0)
      again = on_tool(argc, argv, state, event);
    else if (is_longopt(argv[state->idx]))
      again = on_longopt(argc, argv, options, flags, state, event);
    else if (is_shortopt(argv[state->idx]))
      again = on_shortopt(argc, argv, options, flags, state, event);
    else
      again = on_argument(argc, argv, state, event);
  }

  return 0;
}
