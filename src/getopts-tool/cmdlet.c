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

#include "cmdlet.h"

#define _foreach_cmdlet(head, cmdlet) \
  for ((cmdlet) = (head)->first; (cmdlet) != NULL; (cmdlet) = (cmdlet)->next)

static void cmdlet_head_release(struct nuts_getopts_cmdlet_head* head) {
  nuts_getopts_cmdlet* cmdlet;

  _foreach_cmdlet(head, cmdlet) {
    nuts_getopts_cmdlet_free(cmdlet);
  }
}

static void cmdlet_head_insert(struct nuts_getopts_cmdlet_head* head, nuts_getopts_cmdlet* cmdlet) {
  if (head->first == NULL) {
    head->first = head->last = cmdlet;
  } else {
    head->last->next = cmdlet;
    head->last = cmdlet;
  }
}

static nuts_getopts_cmdlet* cmdlet_head_find(const struct nuts_getopts_cmdlet_head* head, const char* action) {
  nuts_getopts_cmdlet* cmdlet;

  _foreach_cmdlet(head, cmdlet) {
    if (strcmp(cmdlet->action, action) == 0)
      return cmdlet;
  }

  return NULL;
}

nuts_getopts_cmdlet* nuts_getopts_cmdlet_new_standalone(const nuts_getopts_cmdlet* parent, const char* action) {
  // parent = NULL for root-cmdlet
  if (action == NULL || *action == '\0')
    return NULL;

  nuts_getopts_cmdlet* cmdlet;
  char* cmdlet_action;

  cmdlet = malloc(sizeof(struct nuts_getopts_cmdlet_s));
  cmdlet_action = malloc(strlen(action) + 1);

  if (cmdlet == NULL || cmdlet_action == NULL) {
    free(cmdlet);
    free(cmdlet_action);
    return NULL;
  }

  memset(cmdlet, 0, sizeof(struct nuts_getopts_cmdlet_s));
  cmdlet->parent = parent;
  cmdlet->action = strcpy(cmdlet_action, action);

  if (parent != NULL)
    cmdlet->optgroup[0].group = parent->optgroup;

  return cmdlet;
}

nuts_getopts_cmdlet* nuts_getopts_cmdlet_new(nuts_getopts_cmdlet* cmdlet, const char* action) {
  if (cmdlet == NULL || action == NULL)
    return NULL;

  nuts_getopts_cmdlet* child;

  if ((child = cmdlet_head_find(&cmdlet->cmdlets, action)) != NULL)
    return NULL; // already exists

  if ((child = nuts_getopts_cmdlet_new_standalone(cmdlet, action)) == NULL)
    return NULL;

  cmdlet_head_insert(&cmdlet->cmdlets, child);

  return child;
}

void nuts_getopts_cmdlet_free(nuts_getopts_cmdlet* cmdlet) {
  if (cmdlet == NULL)
    return;

  free(cmdlet->action);
  free(cmdlet->syntax);
  free(cmdlet->sdescr);
  free(cmdlet->ldescr);
  nuts_getopts_cmdlet_option_list_release(&cmdlet->options);
  cmdlet_head_release(&cmdlet->cmdlets);

  free(cmdlet);
}

nuts_getopts_cmdlet* nuts_getopts_cmdlet_find(nuts_getopts_cmdlet* cmdlet, const char* action) {
  if (cmdlet != NULL && action != NULL)
    return cmdlet_head_find(&cmdlet->cmdlets, action);
  else
    return NULL;
}

#define _update_attr(cmdlet, attr, val) \
  do {                                                        \
    if (cmdlet->attr == NULL) {                               \
      free(cmdlet->attr);                                     \
      cmdlet->attr = NULL;                                    \
    }                                                         \
                                                              \
    if ((val) != NULL) {                                      \
      if ((cmdlet->attr = malloc(strlen((val)) + 1)) != NULL) \
        cmdlet->attr = strcpy(cmdlet->attr, val);             \
    }                                                         \
  } while (0)

#define _eval_update_attr(cmdlet, attr, val) \
  (((val) == NULL && cmdlet->attr == NULL) || ((val) != NULL && cmdlet->attr != NULL) ? 0 : -1)

int nuts_getopts_cmdlet_set_syntax(nuts_getopts_cmdlet* cmdlet, const char* syntax) {
  if (cmdlet == NULL)
    return -1;

  _update_attr(cmdlet, syntax, syntax);

  return _eval_update_attr(cmdlet, syntax, syntax);
}

int nuts_getopts_cmdlet_set_sdescr(nuts_getopts_cmdlet* cmdlet, const char* sdescr) {
  if (cmdlet == NULL)
    return -1;

  _update_attr(cmdlet, sdescr, sdescr);

  return _eval_update_attr(cmdlet, sdescr, sdescr);
}

int nuts_getopts_cmdlet_set_ldescr(nuts_getopts_cmdlet* cmdlet, const char* ldescr) {
  if (cmdlet == NULL)
    return -1;

  _update_attr(cmdlet, ldescr, ldescr);

  return _eval_update_attr(cmdlet, ldescr, ldescr);
}

nuts_getopts_cmdlet_option* nuts_getopts_cmdlet_add_option(nuts_getopts_cmdlet* cmdlet, const char* lname, char sname, int arg) {
  if (cmdlet == NULL || (lname == NULL && sname == 0))
    return NULL;

  nuts_getopts_cmdlet_option* option = nuts_getopts_cmdlet_option_list_add(&cmdlet->options, lname, sname, arg);

  if (option != NULL) {
    int idx = (cmdlet->parent == NULL) ? 0 : 1;
    cmdlet->optgroup[idx].list = cmdlet->options.opts;
  }

  return option;
}

void nuts_getopts_cmdlet_assign_func(nuts_getopts_cmdlet* cmdlet, nuts_getopts_cmdlet_func func) {
  if (cmdlet == NULL)
    return;

  cmdlet->func = func;
}

int nuts_getopts_cmdlet_invoke(nuts_getopts_cmdlet* cmdlet, nuts_getopts_tool* tool) {
  if (cmdlet == NULL || tool == NULL)
    return -1;

  int rc = 0;

  if (cmdlet->func != NULL)
    rc = cmdlet->func(tool);

  return rc;
}
