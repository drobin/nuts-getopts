# nuts-getopts

The nuts-getopts project implements a zero-copy command line parser; no further
memory allocation is required, no modification of the command line arguments
are made.

## Getting Started

The following code snippet illustrates the usage of the _nuts-getopts_ parser,
which is copied from [src/examples/getopts.c]. Further examples are available
in the [src/examples] directory.

```c
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
```

### Prerequisites

No special prerequisites are necessary, a C99 compliant compiler should be sufficient.

### Compiling and installing

[CMake] is used to build the library.

```sh
mkdir build
cd build
cmake ..
make
make install
```

## License

This project is licensed under the MIT License - see the [LICENSE] file for details

[src/examples/getopts.c]: https://github.com/drobin/nuts-getopts/tree/master/src/examples/getopts.c
[src/examples]: https://github.com/drobin/nuts-getopts/tree/master/src/examples/getopts.c
[LICENSE]: https://github.com/drobin/nuts-getopts/blob/master/LICENSE
[CMake]: http://www.cmake.org
