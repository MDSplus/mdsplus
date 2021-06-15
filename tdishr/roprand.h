#define f_float_exp(val) ((*(int *)val >> 7) & 0xff)
#define f_float_sign(val) ((*(int *)val >> 15) & 0x1)
#define IsRoprandF(val) ((f_float_exp(val) == 0) && (f_float_sign(val) == 1))
#define s_float_exp(val) ((*(int *)val >> 23) & 0xff)
#define IsRoprandS(val) (s_float_exp(val) == 255)
#define IsRoprandD(val) IsRoprandF(val)
#define g_float_exp(val) ((*(int *)val >> 4) & 0x7ff)
#define g_float_sign(val) ((*(int *)val >> 15) & 0x1)
#define IsRoprandG(val) ((g_float_exp(val) == 0) && (g_float_sign(val) == 1))
#define t_float_exp(val) (((*(int64_t *)val) >> 52) & 0x7ff)
#define IsRoprandT(val) (t_float_exp(val) == 2047)

#define IsRoprand(dtype, val)                 \
  ((dtype == DTYPE_F)                         \
       ? IsRoprandF(val)                      \
       : ((dtype == DTYPE_D)                  \
              ? IsRoprandD(val)               \
              : ((dtype == DTYPE_FS)          \
                     ? IsRoprandS(val)        \
                     : ((dtype == DTYPE_G)    \
                            ? IsRoprandG(val) \
                            : ((dtype == DTYPE_FT) ? IsRoprandT(val) : 1)))))
