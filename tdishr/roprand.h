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
#define f_float_exp(val) ((*(int *)val >> 7) & 0xff)
#define f_float_sign(val) ((*(int *)val >> 15) &0x1)
#define IsRoprandF(val) ((f_float_exp(val) == 0) && (f_float_sign(val) == 1))
#define s_float_exp(val) ((*(int *)val >> 23) & 0xff)
#define IsRoprandS(val) (s_float_exp(val) == 255)
#define IsRoprandD(val) IsRoprandF(val)
#define g_float_exp(val) ((*(int *)val >> 4) & 0x7ff)
#define g_float_sign(val) ((*(int *)val >> 15) &0x1)
#define IsRoprandG(val) ((g_float_exp(val) == 0) && (g_float_sign(val) == 1))
#define t_float_exp(val) (((*(int64_t *)val) >> 52) & 0x7ff)
#define IsRoprandT(val) (t_float_exp(val) == 2047)

#define IsRoprand(dtype,val) ((dtype == DTYPE_F) ? IsRoprandF(val) : \
                              ((dtype == DTYPE_D) ? IsRoprandD(val) : \
                               ((dtype == DTYPE_FS) ? IsRoprandS(val) : \
                                ((dtype == DTYPE_G) ? IsRoprandG(val) : \
                                 ((dtype == DTYPE_FT) ? IsRoprandT(val) : 1)))))
