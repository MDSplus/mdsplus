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

#ifndef CHECK_PACK_H
#define CHECK_PACK_H


enum ck_msg_type
{
    CK_MSG_CTX,
    CK_MSG_FAIL,
    CK_MSG_LOC,
    CK_MSG_DURATION,
    CK_MSG_LAST
};

typedef struct CtxMsg
{
    enum ck_result_ctx ctx;
} CtxMsg;

typedef struct LocMsg
{
    int line;
    char *file;
} LocMsg;

typedef struct FailMsg
{
    int  rtype;
    char *msg;
} FailMsg;

typedef struct DurationMsg
{
    int duration;
} DurationMsg;

typedef union
{
    CtxMsg ctx_msg;
    FailMsg fail_msg;
    LocMsg loc_msg;
    DurationMsg duration_msg;
} CheckMsg;

typedef struct RcvMsg
{
    enum ck_result_ctx lastctx;
    enum ck_result_ctx failctx;
    char *fixture_file;
    int fixture_line;
    char *test_file;
    int test_line;
    char *msg;
    int duration;
    int rtype;
} RcvMsg;

void rcvmsg_free(RcvMsg * rmsg);


int pack(enum ck_msg_type type, char **buf, CheckMsg * msg);
int upack(char *buf, CheckMsg * msg, enum ck_msg_type *type);

void ppack(FILE * fdes, enum ck_msg_type type, CheckMsg * msg);
RcvMsg *punpack(FILE * fdes);

#endif /*CHECK_PACK_H */
