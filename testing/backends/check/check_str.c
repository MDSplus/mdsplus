/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*
 * Check: a unit test framework for C
 * Copyright (C) 2001, 2002 Arien Malec
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "lib/libcompat.h"

#include <stdio.h>
#include <stdarg.h>

#include "check.h"
#include "check_list.h"
#include "check_error.h"
#include "check_impl.h"
#include "check_str.h"

static const char *tr_type_str(TestResult * tr);
static int percent_passed(TestStats * t);

char *tr_str(TestResult * tr)
{
    const char *exact_msg;
    char *rstr;

    exact_msg = (tr->rtype == CK_ERROR) ? "(after this point) " : "";

    rstr = ck_strdup_printf("%s:%d:%s:%s:%s:%d: %s%s",
                            tr->file, tr->line,
                            tr_type_str(tr), tr->tcname, tr->tname, tr->iter,
                            exact_msg, tr->msg);

    return rstr;
}

char *tr_short_str(TestResult * tr)
{
    const char *exact_msg;
    char *rstr;

    exact_msg = (tr->rtype == CK_ERROR) ? "(after this point) " : "";

    rstr = ck_strdup_printf("%s:%d: %s%s",
                            tr->file, tr->line, exact_msg, tr->msg);

    return rstr;
}

char *sr_stat_str(SRunner * sr)
{
    char *str;
    TestStats *ts;

    ts = sr->stats;

    str = ck_strdup_printf("%d%%: Checks: %d, Failures: %d, Errors: %d",
                           percent_passed(ts), ts->n_checked, ts->n_failed,
                           ts->n_errors);

    return str;
}

char *ck_strdup_printf(const char *fmt, ...)
{
    /* Guess we need no more than 100 bytes. */
    int n;
    size_t size = 100;
    char *p;
    va_list ap;

    p = (char *)emalloc(size);

    while(1)
    {
        /* Try to print in the allocated space. */
        va_start(ap, fmt);
        n = vsnprintf(p, size, fmt, ap);        
        va_end(ap);
        /* If that worked, return the string. */
        if(n > -1 && n < (int)size)
            return p;

        /* Else try again with more space. */
        if(n > -1)              /* C99 conform vsnprintf() */
            size = (size_t) n + 1;      /* precisely what is needed */
        else                    /* glibc 2.0 */
            size *= 2;          /* twice the old size */

        p = (char *)erealloc(p, size);
    }
}

static const char *tr_type_str(TestResult * tr)
{
    const char *str = NULL;

    if(tr->ctx == CK_CTX_TEST)
    {
        if(tr->rtype == CK_PASS)
            str = "P";
        else if(tr->rtype == CK_FAILURE)
            str = "F";
        else if(tr->rtype == CK_ERROR)
            str = "E";
    }
    else
        str = "S";

    return str;
}

static int percent_passed(TestStats * t)
{
    if(t->n_failed == 0 && t->n_errors == 0)
        return 100;
    else if(t->n_checked == 0)
        return 0;
    else
        return (int)((float)(t->n_checked - (t->n_failed + t->n_errors)) /
                     (float)t->n_checked * 100);
}
