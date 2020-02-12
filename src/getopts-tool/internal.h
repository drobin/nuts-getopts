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

#ifndef NUTS_GETOPTS_TOOL_INTERNAL_H
#define NUTS_GETOPTS_TOOL_INTERNAL_H

#include "nuts-getopts-tool.h"

struct nuts_getopts_cmdlet_head {
  struct nuts_getopts_cmdlet_s* first;
  struct nuts_getopts_cmdlet_s* last;
};

struct nuts_getopts_cmdlet_options {
  int nopts;
  struct nuts_getopts_option* opts;
  char** descr;
};

struct nuts_getopts_cmdlet_s {
  const nuts_getopts_cmdlet* parent;
  char* action;
  char* syntax;
  char* sdescr;
  char* ldescr;
  struct nuts_getopts_cmdlet_options options;
  struct nuts_getopts_option_group optgroup[3];
  struct nuts_getopts_cmdlet_head cmdlets;
  nuts_getopts_cmdlet_func func;
  struct nuts_getopts_cmdlet_s* next;
};

struct nuts_getopts_tool_s {
  nuts_getopts_cmdlet* root;
  struct nuts_getopts_event* events;
  int nevents;
};

int nuts_getopts_cmdlet_options_add(struct nuts_getopts_cmdlet_options* options, const char* lname, char sname, nuts_getopts_argument_type arg, const char* descr);
void nuts_getopts_cmdlet_options_release(struct nuts_getopts_cmdlet_options* options);

nuts_getopts_cmdlet* nuts_getopts_cmdlet_new_standalone(const nuts_getopts_cmdlet* parent, const char* action);
void nuts_getopts_cmdlet_free(nuts_getopts_cmdlet* cmdlet);
int nuts_getopts_cmdlet_invoke(nuts_getopts_cmdlet* cmdlet, nuts_getopts_tool* tool);

void nuts_getopts_cmdlet_head_init(struct nuts_getopts_cmdlet_head* head);
void nuts_getopts_cmdlet_head_release(struct nuts_getopts_cmdlet_head* head);
void nuts_getopts_cmdlet_head_insert(struct nuts_getopts_cmdlet_head* head, nuts_getopts_cmdlet* cmdlet);
nuts_getopts_cmdlet* nuts_getopts_cmdlet_head_find(const struct nuts_getopts_cmdlet_head* head, const char* action);

int nuts_getopts_help(nuts_getopts_tool* tool);

#endif  /* NUTS_GETOPTS_TOOL_INTERNAL_H */
