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

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "cmdlet.h"
#include "help.h"
#include "queue.h"
#include "tool.h"

#define _print_null(s) (((s) != NULL) ? (s) : "")
#define _strlen_null(s) (((s) != NULL) ? strlen((s)) : (0))

struct option_entry {
  const struct nuts_getopts_option* option;
  const nuts_getopts_cmdlet_option* copt;
  SIMPLEQ_ENTRY(option_entry) entries;
};

SIMPLEQ_HEAD(option_head, option_entry);

static struct option_entry* option_entry_new(const struct nuts_getopts_option* option, const nuts_getopts_cmdlet_option* copt) {
  struct option_entry* entry = malloc(sizeof(struct option_entry));

  if (entry == NULL)
    return 0;

  memset(entry, 0, sizeof(struct option_entry));
  entry->option = option;
  entry->copt = copt;

  return entry;
}

static void option_entry_collect(const nuts_getopts_cmdlet* cmdlet, struct option_head* head) {
  for (int i = 0; i < cmdlet->nopts; i++) {
    struct option_entry* entry = option_entry_new(&cmdlet->opts[i], &cmdlet->cmdlet_opts[i]);
    SIMPLEQ_INSERT_TAIL(head, entry, entries);
  }

  if (cmdlet->parent != NULL)
    option_entry_collect(cmdlet->parent, head);
}

static void option_entry_free(struct option_head* head) {
  while (!SIMPLEQ_EMPTY(head)) {
    struct option_entry* entry = SIMPLEQ_FIRST(head);
    SIMPLEQ_REMOVE_HEAD(head, entries);
    free(entry);
  }
}

static void print_spaces(int n) {
  char space[n];

  memset(space, ' ', n);
  printf("%.*s", n, space);
}

static const char* skip_ws(const char* str) {
  const char* p = str;

  while (isspace(*p) && *p != '\0')
    p++;

  return p;
}

static int find_ws(const char* str, int max) {
  int len = max;

  for (; len >= 0; len--) {
    if (isspace(str[len]))
      break;;
  }

  return len;
}

static void println(const char* str, int max, int lpad1, int ldap2) {
  const char* p = skip_ws(str);

  if (strlen(p) > max) {
    int len = find_ws(p, max);
    print_spaces(lpad1);
    printf("%.*s\n", len, p);
    println(p + len, max, ldap2, ldap2);
  }
  else {
    print_spaces(lpad1);
    printf("%s\n", p);
  }
}

static void print_syntax(const nuts_getopts_tool* tool, const nuts_getopts_cmdlet* cmdlet) {
  const char* tool_name = nuts_getopts_tool_name(tool);

  if (cmdlet->parent != NULL) {
    if (cmdlet->syntax != NULL)
      printf("%s %s\n", tool_name, cmdlet->syntax);
    else
      printf("%s %s [options...]\n", tool_name, cmdlet->action);
  } else
    printf("%s [options...] <actions...>\n", tool_name);
}

static void print_descr(const nuts_getopts_cmdlet* cmdlet) {
  if (cmdlet->sdescr != NULL)
    printf("\n%s\n", cmdlet->sdescr);

  if (cmdlet->ldescr != NULL)
    printf("\n%s\n", cmdlet->ldescr);
}

static void print_actions(const nuts_getopts_cmdlet* cmdlet) {
  if (SLIST_EMPTY(&cmdlet->cmdlets))
    return;

  const nuts_getopts_cmdlet* cur;
  int max_len = 0;

  SLIST_FOREACH(cur, &cmdlet->cmdlets, entries) {
    int len = strlen(cur->action);
    if (len > max_len)
      max_len = len;
  }

  printf("\nActions:\n\n");

  SLIST_FOREACH(cur, &cmdlet->cmdlets, entries) {
    if (cur->sdescr != NULL) {
      printf(" %s", cur->action);
      print_spaces(max_len - strlen(cur->action));
      printf(" - %s\n", cur->sdescr);
    } else
      printf(" %s\n", cur->action);
  }
}

static void print_options(const struct option_head* head) {
  printf("\nOptions:\n\n");

  const struct option_entry* entry;
  int max_len = 0;

  SIMPLEQ_FOREACH(entry, head, entries) {
    int lname_len = _strlen_null(entry->option->lname);
    int arg_len = 0;

    if (entry->option->arg == nuts_getopts_required_argument) {
      arg_len = _strlen_null(entry->copt->arg);
      if (arg_len == 0)
        arg_len = 3; // strlen("ARG")
    }

    if (lname_len + arg_len > max_len)
      max_len = lname_len + arg_len;
  }

  SIMPLEQ_FOREACH(entry, head, entries) {
    int len = 0;

    // (1) short option
    if (entry->option->sname != 0)
      printf(" -%c", entry->option->sname);
    else
      printf("   ");

    // (2) (optional) comma separator between short- & long-option
    if (entry->option->sname != 0 && entry->option->lname != NULL)
      printf(", ");
    else
      printf("  ");

    // (3) long option
    if (entry->option->lname != NULL)
      printf("--%s ", entry->option->lname);
    len += _strlen_null(entry->option->lname);

    // (4) argument to option
    if (entry->option->arg == nuts_getopts_required_argument) {
      printf("%s", ((entry->copt->arg != NULL) ? entry->copt->arg : "ARG"));
      len += (entry->copt->arg != NULL) ? strlen(entry->copt->arg) : 3;
    }

    // (5) Right-align everything
    print_spaces(max_len - len + 1);

    // (6) description
    println(_print_null(entry->copt->descr), 80 - max_len - 8, 0, max_len + 9);
  }
}

static const nuts_getopts_cmdlet* cmdlet_detect(nuts_getopts_tool* tool, const nuts_getopts_cmdlet* parent, int idx) {
  const nuts_getopts_cmdlet* cur;

  SLIST_FOREACH(cur, &parent->cmdlets, entries) {
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

  const nuts_getopts_cmdlet* cmdlet = cmdlet_detect(tool, tool->root, idx);
  struct option_head optshead = SIMPLEQ_HEAD_INITIALIZER(optshead);

  option_entry_collect(cmdlet, &optshead);

  print_syntax(tool, cmdlet);
  print_descr(cmdlet);
  print_actions(cmdlet);
  print_options(&optshead);

  option_entry_free(&optshead);

  return 0;
}
