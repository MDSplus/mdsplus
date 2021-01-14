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

enum ck_msg_type {
  CK_MSG_CTX,
  CK_MSG_FAIL,
  CK_MSG_LOC,
  CK_MSG_DURATION,
  CK_MSG_LAST
};

typedef struct CtxMsg {
  enum ck_result_ctx ctx;
} CtxMsg;

typedef struct LocMsg {
  int line;
  char *file;
} LocMsg;

typedef struct FailMsg {
  int rtype;
  char *msg;
} FailMsg;

typedef struct DurationMsg {
  int duration;
} DurationMsg;

typedef union {
  CtxMsg ctx_msg;
  FailMsg fail_msg;
  LocMsg loc_msg;
  DurationMsg duration_msg;
} CheckMsg;

typedef struct RcvMsg {
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

void rcvmsg_free(RcvMsg *rmsg);

int pack(enum ck_msg_type type, char **buf, CheckMsg *msg);
int upack(char *buf, CheckMsg *msg, enum ck_msg_type *type);

void ppack(FILE *fdes, enum ck_msg_type type, CheckMsg *msg);
RcvMsg *punpack(FILE *fdes);

#endif /*CHECK_PACK_H */
