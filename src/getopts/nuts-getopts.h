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

/**
 * A zero-copy command line parser.
 *
 * @file nuts-getopts.h
 *
 * ## Introduction
 *
 * The following sections gives you an overview of features of the
 * _nuts-getopts_ commandline parser.
 *
 * ### Options
 *
 * Command line arguments, which starts with `-` or `--` are recognized as an
 * option. If the option is defined by an nuts_getopts_option entry,
 * nuts_getopts() emits an {@link nuts_getopts_event event} of type
 * #nuts_getopts_option_event. An option can have an argument, if
 * nuts_getopts_option#arg is set to #nuts_getopts_required_argument.
 *
 * An option can have a short name. It starts with `-` (single dash) followed
 * by a single character. If the option has an argument, then the argument
 * directly follows the option character.
 *
 * @code{.sh}
 * # The sample-tool is called with a short option (v),
 * # which does not have an argument.
 * sample-tool -v
 *
 * # The sample-tool is called with a short option (v),
 * # which requires an argument (1).
 * sample-tool -v1
 * @endcode
 *
 * An option can also define a long option. It starts with `--` (double dash)
 * followed by one ore more characters. If the option has an argument, then the
 * argument is passed to the option using the  assignment operator (`=`).
 *
 * @code{.sh}
 * # The sample-tool is called with a long option (verbose),
 * # which does not have an argument.
 * sample-tool --verbose
 *
 * # The sample-tool is called with a long option (verbose),
 * # which requires an argument (1).
 * sample-tool --verbose=1
 * @endcode
 *
 * ### Arguments
 *
 * Command line arguments, which are not bounded to an option are called
 * arguments. If an argument is detected, nuts_getopts() emits an
 * {@link nuts_getopts_event event} of type #nuts_getopts_argument_event.
 *
 * @code{.sh}
 * # The sample-tool is called without any options.
 * # The command line argument `makeitso` is treated as an argument.
 * sample-tool makeitso
 *
 * # The sample-tool is called with the long option `verbose`.
 * # The command line argument `makeitso` is treated as an argument.
 * sample-tool makeitso --verbose
 * @endcode
 *
 * ### Tool
 *
 * The first command line argument usually contains the name of the binary.
 * nuts_getopts() emits an {@link nuts_getopts_event event} of type
 * #nuts_getopts_tool_event for the first command line argument. The event
 * reports a `basename(3)` like name of the tool.
 *
 * @code{.sh}
 * # In this sample nuts_getopts() emits a nuts_getopts_tool_event event
 * # with tool = `sample-tool`.
 * /usr/bin/sample-tool makeitso --verbose
 * @endcode
 *
 * ### Error handling
 *
 * In case of an invalid command line argument nuts_getopts() emits an
 * {@link nuts_getopts_event event} of type #nuts_getopts_error_event. The
 * event contais the invalid command line argument and a
 * {@link nuts_getopts_error_type reason}.
 *
 * ## General usage
 *
 * The function nuts_getopts() and nuts_getopts_group() should run in a loop
 * until `-1` is returned. If the return value is `-1` all command line
 * arguments were parsed. It requires the following arguments:
 *
 * ### `options`
 *
 * An array which contains all supported options. The last entry of the array must contain only zeros.
 *
 * @code
 * // Defines three options:
 * //
 * // 1. The first option requires an argument and has a short name `-v` and a long name `--verbose`.
 * // 2. The second option has no argument and has only a long name `--quiet`.
 * // 3. The third option requires an argument and has only a short name `-f`.
 * //
 * // The last elment in the array only contains zeros.
 * const struct nuts_getopts_option options[] = {
 *   { 'v', "verbose",  nuts_getopts_required_argument },
 *   {  0,  "quiet",    nuts_getopts_no_argument },
 *   { 'f', NULL,       nuts_getopts_required_argument },
 *   { 0 }
 * };
 * @endcode
 *
 * ### `groups`
 *
 * The function nuts_getopts_group() uses the nuts_getopts_option_group
 * structure to configure its options. With an option group you can put
 * several option arrays together.
 *
 * Putting nuts_getopts_option structures together over the
 * nuts_getopts_option_group#list member creates a linear list of options.
 * You can also put option groups into an nuts_getopts_option_group element by
 * using the nuts_getopts_option_group#group member. So you can create a tree
 * of options.
 *
 * The advantage in using option groups is, that you can configure options
 * depending on some external state.
 *
 * ### `flags`
 *
 * Flags passed to the parser.
 *
 * ### `state`
 *
 * The state of the parser. The nuts_getopts_state instance has to filled with
 * zeroes before the first invocation of nuts_getopts(). Don't touch the state
 * afterwards, nuts_getopts() stores its internal state in the variable.
 *
 * ### `event`
 *
 * If nuts_getopts() returns `0`, the parser stores the next event in this
 * variable. You only need to read the variable after a successful
 * nuts_getopts() invocation. The parser will re-initialize its content with
 * each invation of nuts_getopts().
 *
 * ## Example
 *
 * * {@link getopts.c} is an example of how to use nuts_getopts().
 * * {@link getopts_group.c} is an example of how to use nuts_getopts_group().
 */

/**
 * Events emitted by _nuts-getopts_.
 *
 * The enumeration defines possible {@link nuts_getopts_event#type types} of an
 * #nuts_getopts_error_event.
 */
typedef enum {
  /**
   * The name of the command line tool was detected.
   */
  nuts_getopts_tool_event,

  /**
   * An option was deteced.
   */
  nuts_getopts_option_event,

  /**
   * An argument was detected.
   */
  nuts_getopts_argument_event,

  /**
   * An error occured.
   */
  nuts_getopts_error_event
} nuts_getopts_event_type;

/**
 * Argument types supported by an #nuts_getopts_option.
 */
typedef enum {
  /**
   * The option does not have an argument.
   */
  nuts_getopts_no_argument,

  /**
   * The option requires an argument.
   */
  nuts_getopts_required_argument
} nuts_getopts_argument_type;

/**
 * Error types supported by _nuts-getopts_.
 *
 * If the {@link nuts_getopts_event#type type} of an event is
 * #nuts_getopts_error_event, then one of the following errors can occur.
 */
typedef enum {
  /**
   * An option was detected, which is not configured for the parser.
   */
  nuts_getopts_invalid_option,

  /**
   * The option was configured to have an argument; the option-argument is
   * missing.
   */
  nuts_getopts_missing_value,

  /**
   * The option was configured not to have an argument; an option-argument was
   * detected.
   */
  nuts_getopts_needless_value
} nuts_getopts_error_type;

/**
 * Flags which can be passed to nuts_getopts() resp. nuts_getopts_group().
 */
typedef enum {
  /**
   * Ignore unknown options.
   *
   * The parser should skip unknown options rather than raising an error event.
   * An option is undefined when it is not defined as an nuts_getopts_option
   * entry.
   */
  nuts_getopts_ignore_unknown_options = 0x01
} nuts_getopts_flags;

/**
 * Defines an option.
 *
 * A command line option can have a short name and/or a long name. The
 * option can have an argument.
 */
struct nuts_getopts_option {
  /**
   * The short name of the option.
   *
   * It set to `0` the option does not define a short name.
   */
  char sname;

  /**
   * The long name of the option.
   *
   * It set to `NULL` the option does not define a long name.
   */
  char* lname;

  /**
   * The argument of the option.
   *
   * This flag specifies whether the option has an argument or not.
   */
  nuts_getopts_argument_type arg;
};

/**
 * Defines an option-group.
 *
 * Combine several options into a list or tree of options.
 */
struct nuts_getopts_option_group {
  /**
   * The current option-group element references an option-group array.
   *
   * This creates a tree of options.
   */
  const struct nuts_getopts_option_group* group;

  /**
   * The current option-group element references an option array.
   *
   * This creates a list of options.
   */
  const struct nuts_getopts_option* list;
};

/**
 * An event reported by the parser.
 */
struct nuts_getopts_event {
  /**
   * The type of the event.
   *
   * Depending on the value of this member, one of the members of the following
   * {@link nuts_getopts_event#u union} are filled.
   */
  nuts_getopts_event_type type;

  /**
   * Union contains the payload of the event. Depending of the
   * {@link nuts_getopts_event#type type} of the event, one of the members are
   * filled.
   */
  union {
    /**
     * For a #nuts_getopts_tool_event event: contains the name of the command
     * line tool.
     *
     * A `basename(3)` like name of the tool is reported.
     */
    const char* tool;

    /**
     * For a #nuts_getopts_option_event event: contains the option which was
     * detected.
     */
    struct {
      /**
       * A link to the option, which was configured and passed to
       * nuts_getopts() resp. nuts_getopts_group().
       */
      const struct nuts_getopts_option* option;

      /**
       * The argument of the option.
       *
       * If the option was configured to have an argument, then this member
       * contains the value. For an option without an argument, this member is
       * always `NULL`.
       */
      const char* value;
    } opt;

    /**
     * For a #nuts_getopts_argument_event event: contains the argument which
     * was detected.
     */
    const char* arg;

    /**
     * For a #nuts_getopts_error_event event: encodes the error.
     */
    struct {
      /**
       * The concrete error code.
       */
      nuts_getopts_error_type type;

      /**
       * The option, where the error occured.
       */
      const char* option;

      /**
       * The length of the #option.
       */
      int option_len;
    } err;
  } u;
};

/**
 * The state of the parser.
 *
 * The state of the parser. An instance of this type has to filled with zeroes
 * before the first invocation of nuts_getopts(). Don't touch the state
 * afterwards, nuts_getopts() stores its internal state in the variable.
 *
 * The members of the type is hidden for the public interface, there is no need
 * to modify the variable directly.
 */
typedef struct {
/** @cond SKIP_DOC */
  int idx;
/** @endcond */
} nuts_getopts_state;

/**
 * Calls the _nuts-getopts_ parser.
 *
 * Parses the command line arguments `argc`/`argv` and generates an event placed in the `event` argument.
 * The event contains the option, argument, ... which was detected.
 *
 * @param argc Number of arguments in `argv`.
 * @param argv Command line arguments to be parsed.
 * @param options Array with options, which can be detected by the parser. The
 *                last entry of the array must contain only zeros. Passing
 *                `NULL` to `options` is a convenient value for an empty array
 *                (no options).
 * @param flags Flags, which controls the parser. Multiple flags are OR'ed
 *              together. See #nuts_getopts_flags for a list of supported
 *              flags. If no flags should be specified, `0` must be specified
 *              here.
 * @param state The state of the parser. The nuts_getopts_state instance has to
 *              filled with zeroes before the first invocation of
 *              nuts_getopts(). Don't touch the state afterwards,
 *              nuts_getopts() stores its internal state in the variable.
 * @param event The parser stores the next event in this variable. You only
 *              need to read the variable after a successful nuts_getopts()
 *              invocation. The parser will re-initialize its content with each
 *              invation of nuts_getopts().
 * @return The function returns
 *         * `0`: Another event was generated and placed into the `event`
 *                argument. Another nuts_getopts() invocation is required to
 *                parse the next component.
 *         * `-1`: All command line arguments were parsed. No further
 *                 nuts_getopts() invocations are required.
 */
int nuts_getopts(int argc, char* argv[], const struct nuts_getopts_option* options, int flags, nuts_getopts_state* state, struct nuts_getopts_event* event);

/**
 * Calls the _nuts-getopts_ parser (with grouped options).
 *
 * Parses the command line arguments `argc`/`argv` and generates an event placed in the `event` argument.
 * The event contains the option, argument, ... which was detected.
 *
 * @param argc Number of arguments in `argv`.
 * @param argv Command line arguments to be parsed.
 * @param groups Array with option groups, which can be detected by the parser. The
 *               last entry of the array must contain only zeros. Passing
 *               `NULL` to `groups` is a convenient value for an empty array
 *               (no options).
 * @param flags Flags, which controls the parser. Multiple flags are OR'ed
 *              together. See #nuts_getopts_flags for a list of supported
 *              flags. If no flags should be specified, `0` must be specified
 *              here.
 * @param state The state of the parser. The nuts_getopts_state instance has to
 *              filled with zeroes before the first invocation of
 *              nuts_getopts_group(). Don't touch the state afterwards,
 *              nuts_getopts_group() stores its internal state in the variable.
 * @param event The parser stores the next event in this variable. You only
 *              need to read the variable after a successful
 *              nuts_getopts_group() invocation. The parser will re-initialize
 *              its content with each invation of nuts_getopts_group().
 * @return The function returns
 *         * `0`: Another event was generated and placed into the `event`
 *                argument. Another nuts_getopts_group() invocation is required
 *                to parse the next component.
 *         * `-1`: All command line arguments were parsed. No further
 *                 nuts_getopts_group() invocations are required.
 */
int nuts_getopts_group(int argc, char* argv[], const struct nuts_getopts_option_group* groups, int flags, nuts_getopts_state* state, struct nuts_getopts_event* event);

#ifdef __cplusplus
}
#endif

#endif  /* NUTS_GETOPTS_H */
