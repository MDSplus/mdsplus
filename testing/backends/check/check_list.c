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

#include <stdlib.h>
#include <string.h>

#include "check_list.h"
#include "check_error.h"


enum
{
    LINIT = 1,
    LGROW = 2
};

struct List
{
    unsigned int n_elts;
    unsigned int max_elts;
    int current;                /* pointer to the current node */
    int last;                   /* pointer to the node before END */
    void **data;
};

static void maybe_grow(List * lp)
{
    if(lp->n_elts >= lp->max_elts)
    {
        lp->max_elts *= LGROW;
        lp->data = (void **)erealloc(lp->data, lp->max_elts * sizeof(lp->data[0]));
    }
}

List *check_list_create(void)
{
    List *lp;

    lp = (List *)emalloc(sizeof(List));
    lp->n_elts = 0;
    lp->max_elts = LINIT;
    lp->data = (void **)emalloc(sizeof(lp->data[0]) * LINIT);
    lp->current = lp->last = -1;
    return lp;
}

void check_list_add_front(List * lp, void *val)
{
    if(lp == NULL)
        return;
    maybe_grow(lp);
    memmove(lp->data + 1, lp->data, lp->n_elts * sizeof lp->data[0]);
    lp->last++;
    lp->n_elts++;
    lp->current = 0;
    lp->data[lp->current] = val;
}

void check_list_add_end(List * lp, void *val)
{
    if(lp == NULL)
        return;
    maybe_grow(lp);
    lp->last++;
    lp->n_elts++;
    lp->current = lp->last;
    lp->data[lp->current] = val;
}

int check_list_at_end(List * lp)
{
    if(lp->current == -1)
        return 1;
    else
        return (lp->current > lp->last);
}

void check_list_front(List * lp)
{
    if(lp->current == -1)
        return;
    lp->current = 0;
}


void check_list_free(List * lp)
{
    if(lp == NULL)
        return;

    free(lp->data);
    free(lp);
}

void *check_list_val(List * lp)
{
    if(lp == NULL)
        return NULL;
    if(lp->current == -1 || lp->current > lp->last)
        return NULL;

    return lp->data[lp->current];
}

void check_list_advance(List * lp)
{
    if(lp == NULL)
        return;
    if(check_list_at_end(lp))
        return;
    lp->current++;
}


void check_list_apply(List * lp, void (*fp) (void *))
{
    if(lp == NULL || fp == NULL)
        return;

    for(check_list_front(lp); !check_list_at_end(lp); check_list_advance(lp))
        fp(check_list_val(lp));

}
