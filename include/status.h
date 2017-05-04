#pragma once
#include <mdsshr_messages.h>
#define B_TRUE  1
#define B_FALSE 0
#define C_OK    0
#define C_ERROR 1
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
