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

#ifndef NUTS_GETOPTS_TOOL_H
#define NUTS_GETOPTS_TOOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <nuts-getopts.h>

typedef struct nuts_getopts_tool_s nuts_getopts_tool;
typedef struct nuts_getopts_cmdlet_s nuts_getopts_cmdlet;
typedef int (*nuts_getopts_cmdlet_func)(nuts_getopts_tool* tool);

#define NUTS_GETOPTS_TOOL_RC_REQHELP (1 << 8)

nuts_getopts_tool* nuts_getopts_tool_new();
void nuts_getopts_tool_free(nuts_getopts_tool* tool);
nuts_getopts_cmdlet* nuts_getopts_tool_root_cmdlet(const nuts_getopts_tool* tool);
void nuts_getopts_tool_enable_help(nuts_getopts_tool* tool);
const char* nuts_getopts_tool_name(const nuts_getopts_tool* tool);
const char* nuts_getopts_tool_argument(const nuts_getopts_tool* tool, int idx);
const char* nuts_getopts_tool_get_by_sname(const nuts_getopts_tool* tool, char sname);
const char* nuts_getopts_tool_get_by_lname(const nuts_getopts_tool* tool, const char* lname);
int nuts_getopts_tool_is_set_by_sname(const nuts_getopts_tool* tool, char sname);
int nuts_getopts_tool_is_set_by_lname(const nuts_getopts_tool* tool, const char* lname);

int nuts_getopts_tool_run(nuts_getopts_tool* tool, int argc, char* argv[]);

nuts_getopts_cmdlet* nuts_getopts_cmdlet_new(nuts_getopts_cmdlet* cmdlet, const char* action);
int nuts_getopts_cmdlet_set_syntax(nuts_getopts_cmdlet* cmdlet, const char* syntax);
int nuts_getopts_cmdlet_set_sdescr(nuts_getopts_cmdlet* cmdlet, const char* sdescr);
int nuts_getopts_cmdlet_set_ldescr(nuts_getopts_cmdlet* cmdlet, const char* ldescr);
int nuts_getopts_cmdlet_add_option(nuts_getopts_cmdlet* cmdlet, const char* lname, char sname, nuts_getopts_argument_type arg, const char* descr);
void nuts_getopts_cmdlet_assign_func(nuts_getopts_cmdlet* cmdlet, nuts_getopts_cmdlet_func func);

#ifdef __cplusplus
}
#endif

#endif  /* NUTS_GETOPTS_TOOL_H */
