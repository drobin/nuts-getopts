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
#include <stdio.h>

#include "cmdlet.h"
#include "convert.h"
#include "help.h"
#include "tool.h"

static void print_error_event(const struct nuts_getopts_event* event) {
  if (event->type == nuts_getopts_error_event) {
    switch (event->u.err.type) {
      case nuts_getopts_invalid_option:
        fprintf(stderr, "invalid option: %.*s\n", event->u.err.option_len, event->u.err.option);
        break;
      case nuts_getopts_missing_value:
        fprintf(stderr, "missing value for option %.*s\n", event->u.err.option_len, event->u.err.option);
        break;
      case nuts_getopts_needless_value:
        fprintf(stderr, "needless value for option %.*s\n", event->u.err.option_len, event->u.err.option);
        break;
    }
  } else
    fprintf(stderr, "trying to error print an event of type %d...\n", event->type);
}

static void print_converter_error(const struct nuts_getopts_event* event) {
  const struct nuts_getopts_option* option = event->u.opt.option;

  if (option->lname != NULL)
    fprintf(stderr, "invalid value for option: %s\n", option->lname);
  else
    fprintf(stderr, "invalid value for option: %c\n", option->sname);
}

static struct nuts_getopts_event_entry* event_new(nuts_getopts_tool* tool) {
  struct nuts_getopts_event_entry* entry = malloc(sizeof(struct nuts_getopts_event_entry));

  if (entry != NULL) {
    memset(entry, 0, sizeof(struct nuts_getopts_event_entry));
    SIMPLEQ_INSERT_TAIL(&tool->ev_head, entry, entries);
  }

  return entry;
}

static void event_free(struct nuts_getopts_event_entry* entry) {
  if (entry->value != NULL) {
    if (entry->converter != NULL)
      entry->converter->free(entry->value);
    else
      free(entry->value);
  }

  free(entry);
}

static const struct nuts_getopts_event_entry* event_find_tool(const nuts_getopts_tool* tool) {
  const struct nuts_getopts_event_entry* entry;

  SIMPLEQ_FOREACH(entry, &tool->ev_head, entries) {
    if (entry->ev.type == nuts_getopts_tool_event)
      return entry;
  }

  return NULL;
}

static const struct nuts_getopts_event_entry* event_find_option(const nuts_getopts_tool* tool, const char* name) {
  const struct nuts_getopts_event_entry* entry;
  const size_t len = (name != NULL) ? strlen(name) : 0;

  SIMPLEQ_FOREACH(entry, &tool->ev_head, entries) {
    if (entry->ev.type == nuts_getopts_option_event) {
      const struct nuts_getopts_option* option = entry->ev.u.opt.option;

      if ((len == 1 && option->sname == name[0]) ||
          (len > 1 && strncmp(option->lname, name, len) == 0))
        return entry;
    }
  }

  return NULL;
}

static const struct nuts_getopts_event_entry* event_find_arg(const nuts_getopts_tool* tool, int idx) {
  const struct nuts_getopts_event_entry* entry;
  int aidx = 0;

  SIMPLEQ_FOREACH(entry, &tool->ev_head, entries) {
    if (entry->ev.type == nuts_getopts_argument_event) {
      if (aidx == idx)
        return entry;
      aidx++;
    }
  }

  return NULL;
}

static struct nuts_getopts_converter_entry* converter_entry_new(int id, const struct nuts_getopts_converter* converter) {
  struct nuts_getopts_converter_entry* entry = malloc(sizeof(struct nuts_getopts_converter_entry));

  if (entry != NULL) {
    memset(entry, 0, sizeof(struct nuts_getopts_converter_entry));
    entry->id = id;
    entry->conv = converter;
  }

  return entry;
}

static nuts_getopts_cmdlet* cmdlet_detect(nuts_getopts_tool* tool, int argc, char* argv[]) {
  nuts_getopts_state state = { 0 };
  struct nuts_getopts_event ev = { 0 };

  nuts_getopts_cmdlet* cur = tool->root;
  int rc;

  while ((rc = nuts_getopts(argc, argv, NULL, nuts_getopts_ignore_unknown_options, &state, &ev)) != -1) {
    if (ev.type == nuts_getopts_argument_event) {
      nuts_getopts_cmdlet* next = nuts_getopts_cmdlet_find(cur, ev.u.arg);

      if (next == NULL)
        break;

      cur = next;
    } else if (ev.type == nuts_getopts_error_event) {
      print_error_event(&ev);
      return NULL;
    }
  }

  return cur;
}

static int cmdlet_parse(nuts_getopts_tool* tool, nuts_getopts_cmdlet* cmdlet, int argc, char* argv[]) {
  nuts_getopts_state state = { 0 };

  while (1) {
    struct nuts_getopts_event_entry* ev_entry = event_new(tool);

    if (ev_entry == NULL)
      return -1;

    if (nuts_getopts_group(argc, argv, cmdlet->optgroup, 0, &state, &ev_entry->ev) == 0) {
      if (ev_entry->ev.type == nuts_getopts_error_event) {
        print_error_event(&ev_entry->ev);
        return -1;
      }

      if (ev_entry->ev.type == nuts_getopts_option_event) {
        const struct nuts_getopts_option* option = ev_entry->ev.u.opt.option;
        const nuts_getopts_cmdlet_option* cmdlet_option = nuts_getopts_cmdlet_find_option(cmdlet, option);

        ev_entry->converter = nuts_getopts_tool_find_converter(tool, cmdlet_option->conv_id);

        if (cmdlet_option != NULL && ev_entry->converter != NULL) {
          int ok = 0;

          ev_entry->value = ev_entry->converter->convert(ev_entry->ev.u.opt.value, &ok);

          if (!ok) {
            print_converter_error(&ev_entry->ev);
            return -1;
          }
        }
      }
    } else
      break;
  }

  return 0;
}

static int invoke_help(nuts_getopts_tool* tool) {
  nuts_getopts_cmdlet* cmdlet = nuts_getopts_cmdlet_find(tool->root, "help");

  if (cmdlet != NULL)
    return nuts_getopts_cmdlet_invoke(cmdlet, tool);
  else
    return 0;
}

nuts_getopts_tool* nuts_getopts_tool_new() {
  nuts_getopts_tool* tool = malloc(sizeof(struct nuts_getopts_tool_s));

  if (tool == NULL)
    return NULL;

  memset(tool, 0, sizeof(struct nuts_getopts_tool_s));
  tool->root = nuts_getopts_cmdlet_new_standalone(tool, NULL, "root");
  SLIST_INIT(&tool->conv_head);
  SIMPLEQ_INIT(&tool->ev_head);

  nuts_getopts_tool_add_converter(tool, nuts_getopts_tool_string_arg, nuts_getopts_string_converter());
  nuts_getopts_tool_add_converter(tool, nuts_getopts_tool_int_arg, nuts_getopts_int_converter());
  nuts_getopts_tool_add_converter(tool, nuts_getopts_tool_size_arg, nuts_getopts_size_converter());

  return tool;
}

void nuts_getopts_tool_free(nuts_getopts_tool* tool) {
  if (tool != NULL) {
    nuts_getopts_cmdlet_free(tool->root);

    while (!SIMPLEQ_EMPTY(&tool->ev_head)) {
      struct nuts_getopts_event_entry* entry = SIMPLEQ_FIRST(&tool->ev_head);
      SIMPLEQ_REMOVE_HEAD(&tool->ev_head, entries);
      event_free(entry);
    }

    while (!SLIST_EMPTY(&tool->conv_head)) {
      struct nuts_getopts_converter_entry* entry = SLIST_FIRST(&tool->conv_head);
      SLIST_REMOVE_HEAD(&tool->conv_head, entries);
      free(entry);
    }

    free(tool);
  }
}

int nuts_getopts_tool_add_converter(nuts_getopts_tool* tool, int id, const struct nuts_getopts_converter* converter) {
  struct nuts_getopts_converter_entry* entry = NULL;

  if (tool != NULL && converter != NULL)
    entry = converter_entry_new(id, converter);

  if (entry != NULL)
    SLIST_INSERT_HEAD(&tool->conv_head, entry, entries);

  return (entry != NULL) ? 0 : -1;
}

const struct nuts_getopts_converter* nuts_getopts_tool_find_converter(const nuts_getopts_tool* tool, int id) {
  struct nuts_getopts_converter_entry* entry = NULL;

  SLIST_FOREACH(entry, &tool->conv_head, entries) {
    if (entry->id == id)
      return entry->conv;
  }

  return NULL;
}

nuts_getopts_cmdlet* nuts_getopts_tool_root_cmdlet(const nuts_getopts_tool* tool) {
  return (tool != NULL) ? tool->root : NULL;
}

void nuts_getopts_tool_enable_help(nuts_getopts_tool* tool) {
  nuts_getopts_cmdlet* help_cmdlet = NULL;

  if (tool != NULL)
    help_cmdlet = nuts_getopts_cmdlet_new(tool->root, "help");

  if (help_cmdlet != NULL) {
    nuts_getopts_cmdlet_set_sdescr(help_cmdlet, "Display usage information about an action.");
    nuts_getopts_cmdlet_assign_func(help_cmdlet, nuts_getopts_help);
  }
}

const char* nuts_getopts_tool_name(const nuts_getopts_tool* tool) {
  if (tool != NULL) {
    const struct nuts_getopts_event_entry* ev = event_find_tool(tool);
    return (ev != NULL) ? ev->ev.u.tool : NULL;
  } else
    return NULL;
}

const char* nuts_getopts_tool_argument(const nuts_getopts_tool* tool, int idx) {
  if (tool != NULL) {
    const struct nuts_getopts_event_entry* ev = event_find_arg(tool, idx);
    return (ev != NULL) ? ev->ev.u.arg : NULL;
  } else
    return NULL;
}

const void* nuts_getopts_tool_value(const nuts_getopts_tool* tool, const char* name) {
  const struct nuts_getopts_event_entry* ev_entry = NULL;

  if (tool != NULL && name != NULL)
    ev_entry = event_find_option(tool, name);

  return (ev_entry != NULL) ? ev_entry->value : NULL;
}

long nuts_getopts_tool_lvalue(const nuts_getopts_tool* tool, const char* name, long def) {
  const void* value = nuts_getopts_tool_value(tool, name);
  return (value != NULL) ? *(long*)value : def;
}

const char* nuts_getopts_tool_svalue(const nuts_getopts_tool* tool, const char* name) {
  const struct nuts_getopts_event_entry* ev_entry = NULL;

  if (tool != NULL && name != NULL)
    ev_entry = event_find_option(tool, name);

  return (ev_entry != NULL) ? ev_entry->ev.u.opt.value : NULL;
}

int nuts_getopts_tool_is_set(const nuts_getopts_tool* tool, const char* name) {
  const struct nuts_getopts_event_entry* ev = NULL;

  if (tool != NULL && name != NULL)
    ev = event_find_option(tool, name);

  return (ev != NULL);
}

int nuts_getopts_tool_run(nuts_getopts_tool* tool, int argc, char* argv[]) {
  if (tool == NULL || (argc > 0 && argv == NULL))
    return -1;

  nuts_getopts_cmdlet* cmdlet = cmdlet_detect(tool, argc, argv);
  int rc;

  if (cmdlet == NULL)
    return -1;

  if ((rc = cmdlet_parse(tool, cmdlet, argc, argv)) != 0)
    return rc;

  rc = nuts_getopts_cmdlet_invoke(cmdlet, tool);

  if (rc & NUTS_GETOPTS_TOOL_RC_REQHELP)
    rc = invoke_help(tool);

  return rc & 0xFF;
}
