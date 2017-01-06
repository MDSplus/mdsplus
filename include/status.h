#pragma once
#include <mdsshr_messages.h>
#define B_TRUE  1
#define B_FALSE 0
#define C_OK    0
#define C_ERROR 1
#define IS_OK(status)     ((status) & SsSUCCESS)
#define STATUS_OK         IS_OK(status)
#define IS_NOT_OK(status) (!IS_OK(status))
#define STATUS_NOT_OK     (!STATUS_OK)
#define INIT_STATUS       int status = MDSplusSUCCESS
#define INIT_STATUS_ERROR int status = MDSplusERROR
#define TO_CODE(status)   (status & (-8))
#define STATUS_TO_CODE    TO_CODE(status)
