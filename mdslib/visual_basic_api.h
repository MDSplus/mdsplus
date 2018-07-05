static const int zero = 0;
EXPORT int __stdcall MdsConnectVB(char *host)
{
  return MdsConnect(host);
}

EXPORT void __stdcall MdsDisconnectVB()
{
  MdsDisconnect();
}

EXPORT int __stdcall MdsCloseVB(char *tree, int *shot)
{
  return MdsClose(tree, shot);
}

EXPORT int __stdcall MdsSetSocketVB(int *newsocket)
{
  return MdsSetSocket(newsocket);
}

EXPORT int __stdcall WINAPI MdsSetDefaultVB(char *node)
{
  return MdsSetDefault(node);
}

EXPORT int __stdcall MdsOpenVB(char *tree, int *shot)
{
  return MdsOpen(tree, shot);
}

EXPORT int __stdcall descr1VB(int *dtype, void *value)
{
  if (*dtype == DTYPE_CSTRING) {
    int len = strlen(value);
    return descr(dtype, value, (int *)&zero, &len);
  } else
    return descr(dtype, value, (int *)&zero);
}

EXPORT int __stdcall descr1VB2(int *dtype, int *len)
{
  return (descr2(dtype, (int *)&zero, len));
}

EXPORT int __stdcall descr2VB(int *dtype, int *num, void *value)
{
  if (*dtype == DTYPE_CSTRING) {
    int len = strlen(value);
    return descr(dtype, value, num, (int *)&zero, &len);
  } else
    return descr(dtype, value, num, (int *)&zero);
}

EXPORT int __stdcall descr2VB2(int *dtype, int *num, int *len)
{
  return (descr2(dtype, num, (int *)&zero, len));
}

EXPORT int __stdcall descr3VB(int *dtype, int *n1, int *n2, void *value)
{
  if (*dtype == DTYPE_CSTRING) {
    int len = strlen(value);
    return descr(dtype, value, n1, n2, (int *)&zero, &len);
  } else
    return descr(dtype, value, n1, n2, (int *)&zero);
}

EXPORT int __stdcall descr3VB2(int *dtype, int *n1, int *n2, int *len)
{
  return (descr2(dtype, n1, n2, (int *)&zero, len));
}

EXPORT int __stdcall descr4VB(int *dtype, int *n1, int *n2, int *n3, void *value)
{
  if (*dtype == DTYPE_CSTRING) {
    int len = strlen(value);
    return descr(dtype, value, n1, n2, n3, (int *)&zero, &len);
  } else
    return descr(dtype, value, n1, n2, n3, (int *)&zero);
}

EXPORT int __stdcall descr4VB2(int *dtype, int *n1, int *n2, int *n3, int *len)
{
  return (descr2(dtype, n1, n2, n3, (int *)&zero, len));
}

EXPORT int __stdcall MdsValue1VB(char *expression, int *ansd, int *retlen)
{
  return MdsValue(expression, ansd, (int *)&zero, retlen);
}

EXPORT int __stdcall MdsValue1VB2(char *expression, int *ansd, void *value, int *retlen)
{
  return MdsValue2(expression, ansd, value, (int *)&zero, retlen);
}

EXPORT int __stdcall MdsValue2VB(char *expression, int *arg1d, int *ansd, int *retlen)
{
  return MdsValue(expression, arg1d, ansd, (int *)&zero, retlen);
}

EXPORT int __stdcall MdsValue2VB2(char *expression, int *arg1d, void *arg1v, int *ansd, void *value,
			int *retlen)
{
  return MdsValue2(expression, arg1d, arg1v, ansd, value, (int *)&zero, retlen);
}

EXPORT int __stdcall MdsValue3VB(char *expression, int *arg1d, int *arg2d, int *ansd, int *retlen)
{
  return MdsValue(expression, arg1d, arg2d, ansd, (int *)&zero, retlen);
}

EXPORT int __stdcall MdsValue3VB2(char *expression, int *arg1d, void *arg1v, int *arg2d, void *arg2v,
			int *ansd, void *value, int *retlen)
{
  return MdsValue2(expression, arg1d, arg1v, arg2d, arg2v, ansd, value, (int *)&zero, retlen);
}

EXPORT int __stdcall MdsPut1VB(char *node, char *expression, int *ansd)
{
  return MdsPut(node, expression, ansd, (int *)&zero);
}

EXPORT int __stdcall MdsPut1VB2(char *node, char *expression, int *ansd, void *value)
{
  return MdsPut2(node, expression, ansd, value, (int *)&zero);
}

EXPORT int __stdcall MdsPut2VB(char *node, char *expression, int *arg1d, int *ansd)
{
  return MdsPut(node, expression, arg1d, ansd, (int *)&zero);
}

EXPORT int __stdcall MdsPut2VB2(char *node, char *expression, int *arg1d, void *arg1v, int *ansd, void *value)
{
  return MdsPut2(node, expression, arg1d, arg1v, ansd, value, (int *)&zero);
}

EXPORT int __stdcall MdsPut3VB(char *node, char *expression, int *arg1d, int *arg2d, int *ansd)
{
  return MdsPut(node, expression, arg1d, arg2d, ansd, (int *)&zero);
}

EXPORT int __stdcall MdsPut3VB2(char *node, char *expression, int *arg1d, void *arg1v, int *arg2d,
		      void *arg2v, int *ansd, void *value)
{
  return MdsPut2(node, expression, arg1d, arg1v, arg2d, arg2v, ansd, value, (int *)&zero);
}
