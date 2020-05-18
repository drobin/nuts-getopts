# Introduction

The following sections gives you an overview of features of the
_nuts-getopts_ commandline parser.

## Options

Command line arguments, which starts with `-` or `--` are recognized as an
option. If the option is defined by an [nuts_getopts_option] entry,
[nuts_getopts()] emits an [event][nuts_getopts_event] of type
[nuts_getopts_option_event][nuts_getopts_event_type]. An option can have an
argument, if [nuts_getopts_option.arg][nuts_getopts_option] is set to
[nuts_getopts_required_argument][nuts_getopts_argument_type].

An option can have a short name. It starts with `-` (single dash) followed
by a single character. If the option has an argument, then the argument
directly follows the option character.

```sh
# The sample-tool is called with a short option (v),
# which does not have an argument.
sample-tool -v

# The sample-tool is called with a short option (v),
# which requires an argument (1).
sample-tool -v1
```

An option can also define a long option. It starts with `--` (double dash)
followed by one ore more characters. If the option has an argument, then the
argument is passed to the option using the  assignment operator (`=`).

```sh
# The sample-tool is called with a long option (verbose),
# which does not have an argument.
sample-tool --verbose

# The sample-tool is called with a long option (verbose),
# which requires an argument (1).
sample-tool --verbose=1
```

## Arguments

Command line arguments, which are not bounded to an option are called
arguments. If an argument is detected, [nuts_getopts()] emits an
[event][nuts_getopts_event] of type
[nuts_getopts_argument_event][nuts_getopts_event_type].

```sh
# The sample-tool is called without any options.
# The command line argument `makeitso` is treated as an argument.
sample-tool makeitso

# The sample-tool is called with the long option `verbose`.
# The command line argument `makeitso` is treated as an argument.
sample-tool makeitso --verbose
```

## Tool

The first command line argument usually contains the name of the binary.
[nuts_getopts()] emits an [event][nuts_getopts_event] of type
[nuts_getopts_tool_event][nuts_getopts_event_type] for the first command line
argument. The event reports a `basename(3)` like name of the tool.

```sh
# In this sample nuts_getopts() emits a nuts_getopts_tool_event event
# with tool = `sample-tool`.
/usr/bin/sample-tool makeitso --verbose
```

## Error handling

In case of an invalid command line argument [nuts_getopts()] emits an
[event][nuts_getopts_event] of type
[nuts_getopts_error_event][nuts_getopts_event_type]. The event contais the
invalid command line argument and a [reason][nuts_getopts_error_type].

# General usage

The function [nuts_getopts()] and [nuts_getopts_group()] should run in a loop
until `-1` is returned. If the return value is `-1` all command line
arguments were parsed. It requires the following arguments:

## `options`

An array which contains all supported options. The last entry of the array must
contain only zeros.

```c
// Defines three options:
//
// 1. The first option requires an argument and has a short name `-v` and a long name `--verbose`.
// 2. The second option has no argument and has only a long name `--quiet`.
// 3. The third option requires an argument and has only a short name `-f`.
//
// The last elment in the array only contains zeros.
const struct nuts_getopts_option options[] = {
  { 'v', "verbose",  nuts_getopts_required_argument },
  {  0,  "quiet",    nuts_getopts_no_argument },
  { 'f', NULL,       nuts_getopts_required_argument },
  { 0 }
};
```

## `groups`

The function [nuts_getopts_group()] uses the [nuts_getopts_option_group]
structure to configure its options. With an option group you can put
several option arrays together.

Putting [nuts_getopts_option] structures together over the
[nuts_getopts_option_group.list][nuts_getopts_option_group] member creates a
linear list of options. You can also put option groups into an
[nuts_getopts_option_group] element by using the
[nuts_getopts_option_group.group][nuts_getopts_option_group] member. So you can
create a tree of options.

The advantage in using option groups is, that you can configure options
depending on some external state.

## `flags`

Flags passed to the parser.

## `state`

The state of the parser. The [nuts_getopts_state] instance has to filled with
zeroes before the first invocation of [nuts_getopts()]. Don't touch the state
afterwards, [nuts_getopts()] stores its internal state in the variable.

## `event`

If [nuts_getopts()] returns `0`, the parser stores the next event in this
variable. You only need to read the variable after a successful
[nuts_getopts()] invocation. The parser will re-initialize its content with
each invation of [nuts_getopts()].

[nuts_getopts()]: api#nuts_getopts
[nuts_getopts_group()]: api#nuts_getopts_group
[nuts_getopts_argument_type]: api#nuts_getopts_argument_type
[nuts_getopts_error_type]: api#nuts_getopts_error_type
[nuts_getopts_event]: api#nuts_getopts_event
[nuts_getopts_event_type]: api#nuts_getopts_event_type
[nuts_getopts_option]: api#nuts_getopts_option
[nuts_getopts_option_group]: api#nuts_getopts_option_group
[nuts_getopts_state]: api#nuts_getopts_state
