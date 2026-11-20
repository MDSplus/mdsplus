# MDSplus local wheelhouse

This directory holds a pip-installable **MDSplus** wheel that points a Python
environment at *this* MDSplus installation's `python/` package (via a small
`.pth`/bootstrap redirect — it does **not** copy the package). It is placed
here so it can be consumed as a normal package source, e.g.

```sh
pip install --find-links "$MDSPLUS_DIR/python/wheelhouse" MDSplus
```

## Why a wheel instead of `PYTHONPATH=$MDSPLUS_DIR/python`?

Setting `PYTHONPATH` makes `import MDSplus` work, but the environment has **no
record** that MDSplus is installed. That has real costs:

- **Dependencies aren't handled.** MDSplus needs `numpy` (and optional extras).
  With `PYTHONPATH` you have to install those yourself, by hand, everywhere.
- **Package managers can't see it.** pip / uv / poetry can't include MDSplus in
  a lockfile, can't detect version conflicts, and can't reason about it at all.
- **Other projects can't depend on it.** A downstream `pyproject.toml` cannot
  declare a dependency on `MDSplus` and have it resolved.

Installing this wheel registers MDSplus as a **real distribution** (with proper
metadata and `Requires-Dist`), so instead:

- **Dependencies are pulled in automatically.** Installing `MDSplus` brings in
  `numpy` (and the `widgets` extra on request) through the normal resolver — no
  separate, out-of-band dependency wrangling.
- **It works in a venv / uv-managed env.** `pip install` (or uv) MDSplus into an
  isolated environment and it "just works."
- **Other pyprojects can depend on it.** A consuming project lists `MDSplus` as a
  dependency and resolves it from this wheelhouse — no per-project or per-CI
  `PYTHONPATH` plumbing.
- **No copy, always in sync.** The wheel is a redirect to this install's
  `python/` dir, so you import the MDSplus that matches the installed C
  libraries; upgrading the install upgrades what the environment imports.

## Using it

### pip

```sh
# MDSplus resolves from the wheelhouse; its deps (numpy, ...) resolve from PyPI.
# Do NOT pass --no-index -- that would block downloading the dependencies.
pip install --find-links "$MDSPLUS_DIR/python/wheelhouse" MDSplus
```

### uv (CLI)

```sh
uv pip install --find-links "$MDSPLUS_DIR/python/wheelhouse" MDSplus
```

### uv (inside another project's `pyproject.toml`)

Two equivalent routes. Both point at the wheelhouse **directory** (no
version-specific wheel filename to track) and take a **literal** path — uv does
not expand environment variables in `pyproject.toml`/`uv.toml`
(see astral-sh/uv#10096).

**Simple — `find-links`** (the wheelhouse is searched alongside PyPI):

```toml
[project]
dependencies = ["MDSplus"]

[tool.uv]
find-links = ["/usr/local/mdsplus/python/wheelhouse"]
```

**Precise — a flat index pinned to MDSplus** (MDSplus comes *only* from the
wheelhouse; everything else, including numpy, from PyPI):

```toml
[project]
dependencies = ["MDSplus"]

[[tool.uv.index]]
name = "mdsplus"
url = "/usr/local/mdsplus/python/wheelhouse"
format = "flat"      # a directory of wheels, i.e. pip's --find-links
explicit = true      # only used for packages pinned to it

[tool.uv.sources]
MDSplus = { index = "mdsplus" }
```

**Relocatable / env-driven:** since `pyproject.toml` paths are literal, to key
off `$MDSPLUS_DIR` use the `UV_FIND_LINKS` environment variable instead
(comma-separated, equivalent to `--find-links`):

```sh
export UV_FIND_LINKS="$MDSPLUS_DIR/python/wheelhouse"
uv sync    # or: uv add MDSplus, uv pip install MDSplus, ...
```

## How it works

The wheel ships only a `MDSplus.pth` that imports a small `_mdsplus_bootstrap`
module. Because it runs at *every* interpreter startup, the startup path is
deliberately minimal and can never fail: it resolves the install (the baked
prefix if it is still present, else `$MDSPLUS_DIR`), sets `MDSPLUS_DIR` to it,
and prepends its `python/` dir to `sys.path` — no subprocess.

The full MDSplus environment is set up lazily on the first `import MDSplus`, and
only when needed (`MDSPLUS_DIR` was rewritten, or the environment isn't sourced
yet): a one-shot import hook sources that install's `setup.sh` — honoring local
`envsyms` — and merges the resulting environment. Set
`MDSPLUS_LOCAL_WHEEL_DISABLE=1` to turn the bootstrap off without uninstalling.

Its dependencies and metadata are copied from the real
`python/MDSplus/pyproject.toml`, so tools like `pip show MDSplus` report the
genuine package information.
