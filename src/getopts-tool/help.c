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

struct option_entry {
  const struct nuts_getopts_option* option;
  const nuts_getopts_cmdlet_option* copt;
  struct option_entry* next;
};

struct option_head {
  struct option_entry* first;
  struct option_entry* last;
};

static struct option_entry* option_entry_new(const struct nuts_getopts_option* option, const nuts_getopts_cmdlet_option* copt) {
  struct option_entry* entry = malloc(sizeof(struct option_entry));

  if (entry == NULL)
    return 0;

  entry->option = option;
  entry->copt = copt;
  entry->next = NULL;

  return entry;
}

static void option_entry_collect(const nuts_getopts_cmdlet* cmdlet, struct option_head* head) {
  for (int i = 0; i < cmdlet->options.nopts; i++) {
    struct option_entry* entry = option_entry_new(&cmdlet->options.opts[i], &cmdlet->options.copts[i]);

    if (head->first != NULL) {
      head->last->next = entry;
      head->last = entry;
    }
    else
      head->first = head->last = entry;
  }

  if (cmdlet->parent != NULL)
    option_entry_collect(cmdlet->parent, head);
}

static void option_entry_free(struct option_head* head) {
  struct option_entry* entry = head->first;

  while (entry != NULL) {
    struct option_entry* next = entry->next;
    free(entry);
    entry = next;
  }
}

static void print_ralign(const char* str, int max) {
  int nspace = max - strlen(str);
  char space[nspace];

  memset(space, ' ', nspace);

  printf("%.*s", nspace, space);
}

static void print_syntax(nuts_getopts_tool* tool, nuts_getopts_cmdlet* cmdlet) {
  const char* tool_name = nuts_getopts_tool_name(tool);

  if (cmdlet->parent != NULL) {
    if (cmdlet->syntax != NULL)
      printf("%s %s\n", tool_name, cmdlet->syntax);
    else
      printf("%s %s [options...]\n", tool_name, cmdlet->action);
  } else
    printf("%s [options...] <actions...>\n", tool_name);
}

static void print_descr(nuts_getopts_cmdlet* cmdlet) {
  if (cmdlet->sdescr != NULL)
    printf("\n%s\n", cmdlet->sdescr);

  if (cmdlet->ldescr != NULL)
    printf("\n%s\n", cmdlet->ldescr);
}

static void print_actions(nuts_getopts_cmdlet* cmdlet) {
  if (cmdlet->cmdlets.first == NULL)
    return;

  int max_len = 0;

  for (const nuts_getopts_cmdlet* cur = cmdlet->cmdlets.first; cur != NULL; cur = cur->next) {
    int len = strlen(cur->action);
    if (len > max_len)
      max_len = len;
  }

  printf("\nActions:\n\n");

  for (const nuts_getopts_cmdlet* cur = cmdlet->cmdlets.first; cur != NULL; cur = cur->next) {

    if (cur->sdescr != NULL) {
      printf(" %s", cur->action);
      print_ralign(cur->action, max_len);
      printf(" - %s\n", cur->sdescr);
    } else
      printf(" %s\n", cur->action);
  }
}

static void print_options(const struct option_head* head) {
  printf("\nOptions:\n\n");

  int max_lname_len = 0;

  for (const struct option_entry* entry = head->first; entry != NULL; entry = entry->next) {
    if (entry->option->lname != NULL) {
      int len = strlen(entry->option->lname);
      if (len > max_lname_len)
        max_lname_len = len;
    }
  }

  for (const struct option_entry* entry = head->first; entry != NULL; entry = entry->next) {
    if (entry->option->sname != 0)
      printf(" -%c", entry->option->sname);
    else
      printf("   ");

    if (entry->option->sname != 0 && entry->option->lname != NULL)
      printf(",");
    else
      printf(" ");

    if (entry->option->lname != NULL) {
      printf(" --%s", entry->option->lname);
      print_ralign(entry->option->lname, max_lname_len);
      printf("  %s\n", entry->copt->descr);
    } else {
      printf("   "); // space minus minus
      print_ralign("", max_lname_len + 2);
      printf("%s\n", entry->copt->descr);
    }
  }
}

static nuts_getopts_cmdlet* cmdlet_detect(nuts_getopts_tool* tool, nuts_getopts_cmdlet* parent, int idx) {
  for (nuts_getopts_cmdlet* cur = parent->cmdlets.first; cur != NULL; cur = cur->next) {
    const char* arg = nuts_getopts_tool_argument(tool, idx);

    if (arg == NULL)
      return parent;

    if (strcmp(cur->action, arg) == 0)
      return cmdlet_detect(tool, cur, idx + 1);
  }

  return parent;
}

int nuts_getopts_help(nuts_getopts_tool* tool) {
  if (tool == NULL)
    return 1;

  int idx = 0;

  const char* arg = nuts_getopts_tool_argument(tool, 0);

  if (arg != NULL && strcmp(arg, "help") == 0) {
    // If the first argument is `help` then skip it. In this case the help is
    // invoked directly over the help cmdlet. If the first argument is not `help`,
    // then is is requested over NUTS_GETOPTS_TOOL_RC_REQHELP.
    idx = 1;
  }

  nuts_getopts_cmdlet* cmdlet = cmdlet_detect(tool, tool->root, idx);
  struct option_head optshead = { 0 };

  option_entry_collect(cmdlet, &optshead);

  print_syntax(tool, cmdlet);
  print_descr(cmdlet);
  print_actions(cmdlet);
  print_options(&optshead);

  option_entry_free(&optshead);

  return 0;
}
