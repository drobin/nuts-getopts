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

#ifndef NUTS_GETOPTS_TOOL_TOOL_H
#define NUTS_GETOPTS_TOOL_TOOL_H

#include <nuts-getopts.h>

#include "nuts-getopts-tool.h"
#include "queue.h"

struct nuts_getopts_event_entry {
  struct nuts_getopts_event ev;
  const struct nuts_getopts_converter* converter;
  void* value;
  SLIST_ENTRY(nuts_getopts_event_entry) entries;
};

struct  nuts_getopts_converter_entry {
  int id;
  const struct nuts_getopts_converter* conv;
  SLIST_ENTRY(nuts_getopts_converter_entry) entries;
};

SLIST_HEAD(nuts_getopts_event_head, nuts_getopts_event_entry);
SLIST_HEAD(nuts_getopts_converter_head, nuts_getopts_converter_entry);

struct nuts_getopts_tool_s {
  nuts_getopts_cmdlet* root;
  struct nuts_getopts_converter_head conv_head;
  struct nuts_getopts_event_head ev_head;
};

const struct nuts_getopts_converter* nuts_getopts_tool_find_converter(const nuts_getopts_tool* tool, int id);

#endif  /* NUTS_GETOPTS_TOOL_TOOL_H */
