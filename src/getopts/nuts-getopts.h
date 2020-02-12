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

#ifndef NUTS_GETOPTS_H
#define NUTS_GETOPTS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  nuts_getopts_tool_event,
  nuts_getopts_option_event,
  nuts_getopts_argument_event,
  nuts_getopts_error_event
} nuts_getopts_event_type;

typedef enum {
  nuts_getopts_no_argument,
  nuts_getopts_required_argument
} nuts_getopts_argument_type;

typedef enum {
  nuts_getopts_invalid_option,
  nuts_getopts_missing_value,
  nuts_getopts_needless_value
} nuts_getopts_error_type;

typedef enum {
  nuts_getopts_ignore_unknown_options = 0x01
} nuts_getopts_flag;

struct nuts_getopts_option {
  char sname;
  char* lname;
  nuts_getopts_argument_type arg;
};

struct nuts_getopts_option_group {
  const struct nuts_getopts_option_group* group;
  const struct nuts_getopts_option* list;
};

struct nuts_getopts_event {
  nuts_getopts_event_type type;

  union {
    const char* tool;

    struct {
      const struct nuts_getopts_option* option;
      const char* value;
    } opt;

    const char* arg;

    struct {
      nuts_getopts_error_type type;
      const char* option;
      int option_len;
    } err;
  } u;
};

typedef struct {
  int idx;
} nuts_getopts_state;

int nuts_getopts(int argc, char* argv[], const struct nuts_getopts_option* options, int flags, nuts_getopts_state* state, struct nuts_getopts_event* event);
int nuts_getopts_group(int argc, char* argv[], const struct nuts_getopts_option_group* options, int flags, nuts_getopts_state* state, struct nuts_getopts_event* event);

#ifdef __cplusplus
}
#endif

#endif  /* NUTS_GETOPTS_H */
