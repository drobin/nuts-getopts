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
#include <stdio.h>

#include <nuts-getopts.h>

static void handle_tool_event(const struct nuts_getopts_event* ev) {
  printf("tool: %s\n", ev->u.tool);
}

static void handle_option_event(const struct nuts_getopts_event* ev) {
  // The options which was selected.
  const struct nuts_getopts_option* option = ev->u.opt.option;

  if (option->arg == nuts_getopts_required_argument) {
    printf("option: %c/%s, arg: %s\n",
      option->sname, option->lname, ev->u.opt.value);
  } else {
    printf("option: %c/%s, no-arg\n",
      option->sname, option->lname);
  }
}

static void handle_argument_event(const struct nuts_getopts_event* ev) {
  printf("argument: %s\n", ev->u.arg);
}

static void handle_error_event(const struct nuts_getopts_event* ev) {
  switch (ev->u.err.type) {
    case nuts_getopts_invalid_option:
      fprintf(stderr, "error: invalid option %.*s\n",
        ev->u.err.option_len, ev->u.err.option);
      break;
    case nuts_getopts_missing_value:
      fprintf(stderr, "error: missing value for option %.*s\n",
        ev->u.err.option_len, ev->u.err.option);
      break;
    case nuts_getopts_needless_value:
      fprintf(stderr, "error: needless value for option %.*s\n",
        ev->u.err.option_len, ev->u.err.option);
      break;
  }
}

int main(int argc, char* argv[]) {
  // Defines three options:
  //
  // 1. The first option requires an argument and has a short name `-v` and a
  //    long name `--verbose`.
  // 2. The second option has no argument and has only a long name `--quiet`.
  // 3. The third option requires an argument and has only a short name `-f`.
  const struct nuts_getopts_option options[] = {
    { 'v', "verbose",  nuts_getopts_required_argument },
    {  0,  "quiet",    nuts_getopts_no_argument },
    { 'f', NULL,       nuts_getopts_required_argument },
    { 0 }
  };

  // The state of the parser.
  // Must be initialized with zeros before the first invocation of
  // nuts_getopts(). Don't modify the variable afterwards, nuts_getopts()
  // stores its internal state in the variable.
  nuts_getopts_state state = { 0 };

  // The event the parser emits.
  // nuts_getopts() stores the next event in this variable. The parser will
  // re-initialize its content with each invation of nuts_getopts().
  struct nuts_getopts_event ev = { 0 };

  // Call the parser in a loop.
  while (nuts_getopts(argc, argv, options, 0, &state, &ev) != -1) {
    // Depending on the event-type call a related handler.
    switch (ev.type) {
      case nuts_getopts_tool_event:
        handle_tool_event(&ev);
        break;
      case nuts_getopts_option_event:
        handle_option_event(&ev);
        break;
      case nuts_getopts_argument_event:
        handle_argument_event(&ev);
        break;
      case nuts_getopts_error_event:
        handle_error_event(&ev);
        return 1;
    }
  }

  return 0;
}
