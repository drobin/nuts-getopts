## nuts-getopts API

### Enumerations

#### nuts_getopts_event_type

```
enum nuts_getopts_event_type {
  nuts_getopts_tool_event,
  nuts_getopts_option_event,
  nuts_getopts_argument_event,
  nuts_getopts_error_event
};
```

Events emitted by nuts-getopts.

The enumeration defines possible types of an nuts_getopts_error_event.

| `nuts_getopts_tool_event` | The name of the command line tool was detected. |
| `nuts_getopts_option_event` | An option was deteced. |
| `nuts_getopts_argument_event` | An argument was detected. |
| `nuts_getopts_error_event` | An error occured. |


#### nuts_getopts_argument_type

```
enum nuts_getopts_argument_type {
  nuts_getopts_no_argument,
  nuts_getopts_required_argument
};
```

Argument types supported by an nuts_getopts_option.



| `nuts_getopts_no_argument` | The option does not have an argument. |
| `nuts_getopts_required_argument` | The option requires an argument. |


#### nuts_getopts_error_type

```
enum nuts_getopts_error_type {
  nuts_getopts_invalid_option,
  nuts_getopts_missing_value,
  nuts_getopts_needless_value
};
```

Error types supported by nuts-getopts.

If the type of an event is nuts_getopts_error_event, then one of the following errors can occur.

| `nuts_getopts_invalid_option` | An option was detected, which is not configured for the parser. |
| `nuts_getopts_missing_value` | The option was configured to have an argument; the option-argument is missing. |
| `nuts_getopts_needless_value` | The option was configured not to have an argument; an option-argument was detected. |


#### nuts_getopts_flags

```
enum nuts_getopts_flags {
  nuts_getopts_ignore_unknown_options
};
```

Flags which can be passed to nuts_getopts() resp.

nuts_getopts_group().

| `nuts_getopts_ignore_unknown_options` | Ignore unknown options. |


### Data types

#### nuts_getopts_state

```
nuts_getopts_state
```

The state of the parser.

The state of the parser. An instance of this type has to filled with zeroes before the first invocation of nuts_getopts(). Don't touch the state afterwards, nuts_getopts() stores its internal state in the variable.
The members of the type is hidden for the public interface, there is no need to modify the variable directly.

### Data structures

#### nuts_getopts_option

```
struct nuts_getopts_option {
  char sname;
  char * lname;
  nuts_getopts_argument_type arg;
};
```

Defines an option.

A command line option can have a short name and/or a long name. The option can have an argument.

#### nuts_getopts_option_group

```
struct nuts_getopts_option_group {
  const struct nuts_getopts_option_group * group;
  const struct nuts_getopts_option * list;
};
```

Defines an option-group.

Combine several options into a list or tree of options.

#### nuts_getopts_event

```
struct nuts_getopts_event {
  nuts_getopts_event_type type;
  const char * tool;
  const struct nuts_getopts_option * option;
  const char * value;
  struct nuts_getopts_event::@0::@1 opt;
  const char * arg;
  nuts_getopts_error_type type;
  const char * option;
  int option_len;
  struct nuts_getopts_event::@0::@2 err;
  union nuts_getopts_event::@0 u;
};
```

An event reported by the parser.



### Functions

#### nuts_getopts()

```
int nuts_getopts(
  int argc,
  char * argv[],
  const struct nuts_getopts_option * options,
  int flags,
  nuts_getopts_state * state,
  struct nuts_getopts_event * event
);
```

Calls the nuts-getopts parser.

Parses the command line arguments argc/argv and generates an event placed in the event argument. The event contains the option, argument, ... which was detected.


argc


Number of arguments in argv. 




argv


Command line arguments to be parsed. 




options


Array with options, which can be detected by the parser. The last entry of the array must contain only zeros. Passing NULL to options is a convenient value for an empty array (no options). 




flags


Flags, which controls the parser. Multiple flags are OR'ed together. See nuts_getopts_flags for a list of supported flags. If no flags should be specified, 0 must be specified here. 




state


The state of the parser. The nuts_getopts_state instance has to filled with zeroes before the first invocation of nuts_getopts(). Don't touch the state afterwards, nuts_getopts() stores its internal state in the variable. 




event


The parser stores the next event in this variable. You only need to read the variable after a successful nuts_getopts() invocation. The parser will re-initialize its content with each invation of nuts_getopts(). 



The function returns
0: Another event was generated and placed into the event argument. Another nuts_getopts() invocation is required to parse the next component.
-1: All command line arguments were parsed. No further nuts_getopts() invocations are required.

#### nuts_getopts_group()

```
int nuts_getopts_group(
  int argc,
  char * argv[],
  const struct nuts_getopts_option_group * groups,
  int flags,
  nuts_getopts_state * state,
  struct nuts_getopts_event * event
);
```

Calls the nuts-getopts parser (with grouped options).

Parses the command line arguments argc/argv and generates an event placed in the event argument. The event contains the option, argument, ... which was detected.


argc


Number of arguments in argv. 




argv


Command line arguments to be parsed. 




groups


Array with option groups, which can be detected by the parser. The last entry of the array must contain only zeros. Passing NULL to groups is a convenient value for an empty array (no options). 




flags


Flags, which controls the parser. Multiple flags are OR'ed together. See nuts_getopts_flags for a list of supported flags. If no flags should be specified, 0 must be specified here. 




state


The state of the parser. The nuts_getopts_state instance has to filled with zeroes before the first invocation of nuts_getopts_group(). Don't touch the state afterwards, nuts_getopts_group() stores its internal state in the variable. 




event


The parser stores the next event in this variable. You only need to read the variable after a successful nuts_getopts_group() invocation. The parser will re-initialize its content with each invation of nuts_getopts_group(). 



The function returns
0: Another event was generated and placed into the event argument. Another nuts_getopts_group() invocation is required to parse the next component.
-1: All command line arguments were parsed. No further nuts_getopts_group() invocations are required.
