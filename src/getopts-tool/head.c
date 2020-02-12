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

#include "internal.h"

#define _foreach_cmdlet(head, cmdlet) \
  for ((cmdlet) = (head)->first; (cmdlet) != NULL; (cmdlet) = (cmdlet)->next)

void nuts_getopts_cmdlet_head_init(struct nuts_getopts_cmdlet_head* head) {
  if (head != NULL) {
    head->first = NULL;
    head->last = NULL;
  }
}

void nuts_getopts_cmdlet_head_release(struct nuts_getopts_cmdlet_head* head) {
  if (head != NULL) {
    nuts_getopts_cmdlet* cmdlet;

    _foreach_cmdlet(head, cmdlet) {
      nuts_getopts_cmdlet_free(cmdlet);
    }
  }
}

void nuts_getopts_cmdlet_head_insert(struct nuts_getopts_cmdlet_head* head, nuts_getopts_cmdlet* cmdlet) {
  if (head != NULL && cmdlet != NULL) {
    if (head->first == NULL) {
      head->first = head->last = cmdlet;
    } else {
      head->last->next = cmdlet;
      head->last = cmdlet;
    }
  }
}

nuts_getopts_cmdlet* nuts_getopts_cmdlet_head_find(const struct nuts_getopts_cmdlet_head* head, const char* action) {
  if (head != NULL && action != NULL) {
    nuts_getopts_cmdlet* cmdlet;

    _foreach_cmdlet(head, cmdlet) {
      if (strcmp(cmdlet->action, action) == 0)
        return cmdlet;
    }
  }

  return NULL;
}

