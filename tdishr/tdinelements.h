extern int tdi_n_elements();
#define N_ELEMENTS(dsc_ptr, count) \
  count = tdi_n_elements((const mdsdsc_t *const)(dsc_ptr), &status)
