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
#pragma once
#include <mdsshr_messages.h>
#define B_TRUE  1
#define B_FALSE 0
#define C_OK    0
#define C_ERROR -1
#define IS_OK(status)     ((status) & 1)
#define STATUS_OK         IS_OK(status)
#define IS_NOT_OK(status) (!IS_OK(status))
#define STATUS_NOT_OK     (!STATUS_OK)
#define INIT_STATUS_AS    int status =
#define INIT_STATUS       INIT_STATUS_AS MDSplusSUCCESS
#define INIT_STATUS_ERROR INIT_STATUS_AS MDSplusERROR
#define TO_CODE(status)   (status & (-8))
#define STATUS_TO_CODE    TO_CODE(status)
#define RETURN_IF_STATUS_NOT_OK  if STATUS_NOT_OK return status;
#define RETURN_IF_NOT_OK(CHECK)  do {status = CHECK; RETURN_IF_STATUS_NOT_OK;} while(0)
