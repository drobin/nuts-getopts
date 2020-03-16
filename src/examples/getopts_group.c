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

/**
 * @example getopts_group.c
 *
 * This is an example of how to use nuts_getopts_group().
 *
 *
 * We have a tool, that supports the two options
 * 1. `-v/--verbose`
 * 2. `--quiet`
 *
 * Depending on the first argument, the following options should also be
 * configured:
 *
 * * If the first argument is `one`, then the additional option `-f`/`--file`
 *   should be configured.
 * * If the first argument is `two`, then the additional option `-g`/`--global`
 *   should be configured.
 *
 * @code{.sh}
 * $ nuts-getopts-group-example -v1 --quiet
 * tool: nuts-getopts-group-example
 * option: v/verbose, arg: 1
 * option: /quiet, no-arg
 *
 * $ getopts-group-example one -v1 --quiet -fx
 * tool: nuts-getopts-group-example
 * argument: one
 * option: v/verbose, arg: 1
 * option: /quiet, no-arg
 * option: f/file, arg: x
 *
 * $ nuts-getopts-group-example two -v1 --quiet -g
 * tool: nuts-getopts-group-example
 * argument: two
 * option: v/verbose, arg: 1
 * option: /quiet, no-arg
 * option: g/global, no-arg
 * @endcode
 *
 * But:
 *
 * @code{.sh}
 * $ nuts-getopts-group-example -fx
 * tool: nuts-getopts-group-example
 * error: invalid option -f
 *
 * $ nuts-getopts-group-example -g
 * tool: nuts-getopts-group-example
 * error: invalid option -g
 *
 * $ nuts-getopts-group-example one -g
 * tool: nuts-getopts-group-example
 * argument: one
 * error: invalid option -g
 *
 * $ nuts-getopts-group-example two -fx
 * tool: nuts-getopts-group-example
 * argument: two
 * error: invalid option -f
 * @endcode
 *
 */

#include <stdlib.h>
#include <string.h>
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
  // Define two global options.
  // They are valid - independent from the `action` argument.
  //
  // 1. The first option requires an argument and has a short name `-v` and a
  //    long name `--verbose`.
  // 2. The second option has no argument and has only a long name `--quiet.
  const struct nuts_getopts_option global_options[] = {
    { 'v', "verbose",  nuts_getopts_required_argument },
    {  0,  "quiet",    nuts_getopts_no_argument },
    { 0 }
  };

  // Define an option, which should only be active,
  // when the `action` argument is "one".
  //
  // It requires an argument and has a short name `-f`
  // and a long name `--file`.
  const struct nuts_getopts_option one_options[] = {
    { 'f', "file", nuts_getopts_required_argument },
    { 0 }
  };

  // Define an option, which should only be active,
  // when the `action` argument is "two".
  //
  // It has no argument and has a short name `-g` and a long name `--global`.
  const struct nuts_getopts_option two_options[] = {
    { 'g', "global", nuts_getopts_no_argument },
    { 0 }
  };

  // Put all together in this option-group:
  // The first element always contains the global options.
  // The second element is a placeholder for the `action` specific options.
  // If no second element is assigned, the element is automatically the
  // end-of-group element.
  struct nuts_getopts_option_group all_options[] = {
    { .list = global_options },
    { .list = NULL },
    { 0 }
  };

  // The state of the parser.
  // Must be initialized with zeros before the first invocation of
  // nuts_getopts() resp. nuts_getopts_group(). Don't modify the variable
  // afterwards, nuts_getopts()/nuts_getopts_group()/ stores its internal state
  // in the variable.
  nuts_getopts_state state = { 0 };

  // The event the parser emits.
  // nuts_getopts()/nuts_getopts_group() stores the next event in this
  // variable. The parser will re-initialize its content with each invation of
  // nuts_getopts()/nuts_getopts_group().
  struct nuts_getopts_event ev = { 0 };

  // The action extracted from the command line.
  const char* action = NULL;

  // First we want to extract the action argument from the command line.
  // For this, we ignore all options and are only interested in arguments.
  while (nuts_getopts(argc, argv, NULL, nuts_getopts_ignore_unknown_options, &state, &ev) == 0) {
    if (ev.type == nuts_getopts_argument_event) {
      // The first argument is the action.
      action = ev.u.arg;
      break;
    } else if (ev.type == nuts_getopts_error_event) {
      // Error fall-through
      handle_error_event(&ev);
      return 1;
    }
  }

  // If we have an action, when assign the action-specific options to the
  // action-group.
  if (action != NULL) {
    if (strcmp(action, "one") == 0)
      all_options[1].list = one_options;
    else if (strcmp(action, "two") == 0)
      all_options[1].list = two_options;
  }

  // Reset the state, we start another parser!
  memset(&state, 0, sizeof(nuts_getopts_state));

  // Call the parser in a loop.
  while (nuts_getopts_group(argc, argv, all_options, 0, &state, &ev) == 0) {
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
