# CAMSHR DB Backend Policy

`CAMSHR_DB_BACKEND` controls the CAMSHR database backend.

Default behavior:
- If `CAMSHR_DB_BACKEND` is unset or empty, CAMSHR uses the `sqlite` backend.

Supported values:
- `sqlite` / `sql` / `auto`: use sqlite backend.
- `mmap` / `legacy` / `file`: force legacy memory-mapped flat-file backend.

Notes:
- Legacy `mmap` mode is kept as a fallback compatibility path.
- When forcing legacy mode, CAMSHR prints a one-time warning to `stderr`.
- Unknown `CAMSHR_DB_BACKEND` values fall back to `sqlite` and print a one-time warning.

Testing guidance:
- Sqlite path tests:
  - `camshr/testing/camshr_backend_policy_test`
  - `camshr/testing/camshr_sqlite_schema_test`
  - `camshr/testing/camshr_sqlite_migration_test`
  - `camshr/testing/camshr_sqlite_backend_roundtrip_test`
  - `camshr/testing/camshr_autoconfig_sqlite_fakeproc_test`
- Legacy fallback coverage:
  - `camshr/testing/camshr_crate_state_fakehw_test` (forces `CAMSHR_DB_BACKEND=mmap`)

Native full-test note:
- In full native `ctest` runs, `tditest/testing/test-dev-py` may be skipped if
  `libMitDevices` is not present in `MDSPLUS_LIBRARY_PATH`.
