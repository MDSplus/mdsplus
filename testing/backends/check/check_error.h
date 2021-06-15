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

#ifndef ERROR_H
#define ERROR_H

#include "lib/libcompat.h"
#include <setjmp.h>

extern jmp_buf error_jmp_buffer;

/* Include stdlib.h beforehand */

/* Print error message and die
   If fmt ends in colon, include system error information */
void eprintf(const char *fmt, const char *file, int line,
             ...) CK_ATTRIBUTE_NORETURN;
/* malloc or die */
void *emalloc(size_t n);
void *erealloc(void *, size_t n);

#endif /*ERROR_H */
