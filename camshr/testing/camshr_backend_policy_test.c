#include "testing.h"

#include "../camac_db_backend.h"

#include <stdlib.h>

int main()
{
  BEGIN_TESTING(CamShrBackendPolicy);

  TEST1(unsetenv("CAMSHR_DB_BACKEND") == 0);
  TEST1(camac_db_backend_enabled() != 0);

  TEST1(setenv("CAMSHR_DB_BACKEND", "", 1) == 0);
  TEST1(camac_db_backend_enabled() != 0);

  TEST1(setenv("CAMSHR_DB_BACKEND", "sqlite", 1) == 0);
  TEST1(camac_db_backend_enabled() != 0);

  TEST1(setenv("CAMSHR_DB_BACKEND", "sql", 1) == 0);
  TEST1(camac_db_backend_enabled() != 0);

  TEST1(setenv("CAMSHR_DB_BACKEND", "auto", 1) == 0);
  TEST1(camac_db_backend_enabled() != 0);

  TEST1(setenv("CAMSHR_DB_BACKEND", "mmap", 1) == 0);
  TEST1(camac_db_backend_enabled() == 0);

  TEST1(setenv("CAMSHR_DB_BACKEND", "legacy", 1) == 0);
  TEST1(camac_db_backend_enabled() == 0);

  TEST1(setenv("CAMSHR_DB_BACKEND", "file", 1) == 0);
  TEST1(camac_db_backend_enabled() == 0);

  TEST1(setenv("CAMSHR_DB_BACKEND", "something-unknown", 1) == 0);
  TEST1(camac_db_backend_enabled() != 0);

  TEST1(unsetenv("CAMSHR_DB_BACKEND") == 0);

  END_TESTING;
  return 0;
}
