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
#include <stdio.h>
#include <string.h>
extern void toms510_(float *xin, float *yin, float *err, int *n, float *xout,
                     float *yout, float *w, int *k, int *p);

#define N 5
#define fEQ(a, b, d) (((a > b) ? a - b : b - a) < d)

int main()
{
  float x[N] = {0.f, 1.f, 2.f, 3.f, 4.f};
  float y[N] = {0.f, 1.f, 4.f, 1.f, 0.f};
  float err = .02 * 4; // max(x)-min(x) = 4;
  int n = N;
  float xout[N];
  float yout[N];
  int i;
  for (i = 0; i < n; i++)
  {
    xout[i] = (float)i;
    yout[i] = (float)i;
  }
  float w[N];
  memcpy(w, x, sizeof(x));
  int k = 0;
  int p = 6;
  toms510_(x, y, &err, &n, xout, yout, w, &k, &p);
  float ey[N] = {-.08, 1.08, 3.99781, 1., 0.};
  float ex[N] = {0., 1., 2.0274, 3., 4.};
  for (i = 0; i < N; i++)
  {
    if (!fEQ(ex[i], xout[i], 1e-5f))
      return 1;
    if (!fEQ(ey[i], yout[i], 1e-5f))
      return 1;
  }
  return 0;
}
