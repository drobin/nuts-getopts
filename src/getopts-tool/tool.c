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

#include "internal.h"

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

static struct nuts_getopts_event* event_new(nuts_getopts_tool* tool) {
  struct nuts_getopts_event* new_events = realloc(tool->events, sizeof(struct nuts_getopts_event) * (tool->nevents + 1));

  if (new_events == NULL)
    return NULL;

  struct nuts_getopts_event* ev = &new_events[tool->nevents];
  memset(ev, 0, sizeof(struct nuts_getopts_event));

  tool->events = new_events;
  tool->nevents++;

  return ev;
}

static const struct nuts_getopts_event* event_find_tool(const nuts_getopts_tool* tool) {
  for (int i = 0; i < tool->nevents; i++) {
    if (tool->events[i].type == nuts_getopts_tool_event)
      return &tool->events[i];
  }

  return NULL;
}

static const struct nuts_getopts_event* event_find_option(const nuts_getopts_tool* tool, const char* name) {
  const size_t len = (name != NULL) ? strlen(name) : 0;

  for (int i = 0; i < tool->nevents; i++) {
    if (tool->events[i].type == nuts_getopts_option_event) {
      const struct nuts_getopts_option* option = tool->events[i].u.opt.option;

      if ((len == 1 && option->sname == name[0]) ||
          (len > 1 && strncmp(option->lname, name, len) == 0))
        return &tool->events[i];
    }
  }

  return NULL;
}

static const struct nuts_getopts_event* event_find_arg(const nuts_getopts_tool* tool, int idx) {
  int aidx = 0;

  for (int i = 0; i < tool->nevents; i++) {
    if (tool->events[i].type == nuts_getopts_argument_event) {
      if (aidx == idx)
        return &tool->events[i];
      aidx++;
    }
  }

  return NULL;
}

static nuts_getopts_cmdlet* cmdlet_detect(nuts_getopts_tool* tool, int argc, char* argv[]) {
  nuts_getopts_state state = { 0 };
  struct nuts_getopts_event ev = { 0 };

  nuts_getopts_cmdlet* cur = tool->root;
  int rc;

  while ((rc = nuts_getopts(argc, argv, NULL, nuts_getopts_ignore_unknown_options, &state, &ev)) != -1) {
    if (ev.type == nuts_getopts_argument_event) {
      nuts_getopts_cmdlet* next = nuts_getopts_cmdlet_head_find(&cur->cmdlets, ev.u.arg);

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
    struct nuts_getopts_event* ev = event_new(tool);

    if (ev == NULL)
      return -1;

    if (nuts_getopts_group(argc, argv, cmdlet->optgroup, 0, &state, ev) == 0) {
      if (ev->type == nuts_getopts_error_event) {
        print_error_event(ev);
        return -1;
      }
    } else
      break;
  }

  return 0;
}

static int invoke_help(nuts_getopts_tool* tool) {
  nuts_getopts_cmdlet* cmdlet = nuts_getopts_cmdlet_head_find(&tool->root->cmdlets, "help");

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
  tool->root = nuts_getopts_cmdlet_new_standalone(NULL, "root");

  return tool;
}

void nuts_getopts_tool_free(nuts_getopts_tool* tool) {
  if (tool != NULL) {
    nuts_getopts_cmdlet_free(tool->root);
    free(tool->events);
    free(tool);
  }
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
    const struct nuts_getopts_event* ev = event_find_tool(tool);
    return (ev != NULL) ? ev->u.tool : NULL;
  } else
    return NULL;
}

const char* nuts_getopts_tool_argument(const nuts_getopts_tool* tool, int idx) {
  if (tool != NULL) {
    const struct nuts_getopts_event* ev = event_find_arg(tool, idx);
    return (ev != NULL) ? ev->u.arg : NULL;
  } else
    return NULL;
}

const char* nuts_getopts_tool_value(const nuts_getopts_tool* tool, const char* name) {
  const struct nuts_getopts_event* ev = NULL;

  if (tool != NULL && name != NULL)
    ev = event_find_option(tool, name);

  return (ev != NULL) ? ev->u.opt.value : NULL;
}

int nuts_getopts_tool_is_set(const nuts_getopts_tool* tool, const char* name) {
  const struct nuts_getopts_event* ev = NULL;

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
