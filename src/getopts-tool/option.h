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

#ifndef NUTS_GETOPTS_TOOL_OPTION_H
#define NUTS_GETOPTS_TOOL_OPTION_H

#include "nuts-getopts-tool.h"

struct nuts_getopts_cmdlet_option_s {
  char* descr;
  char* arg;
};

void nuts_getopts_cmdlet_option_init(nuts_getopts_cmdlet_option* option);
void nuts_getopts_cmdlet_option_release(nuts_getopts_cmdlet_option* option);


#endif  /* NUTS_GETOPTS_TOOL_OPTION_H*/
