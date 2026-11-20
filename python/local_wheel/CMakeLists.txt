# Build the MDSplus "local" wheel -- a pip-installable wheel whose only payload
# is a MDSplus.pth (+ a _mdsplus_bootstrap.py it imports) that makes
# `import MDSplus` resolve to the system-installed python/MDSplus package.
#
# The wheel is built at INSTALL time (install(SCRIPT) below) rather than build
# time, so the real install prefix -- ${CMAKE_INSTALL_PREFIX} as resolved at
# install, honoring `cmake --install --prefix ...` -- can be baked into the
# bootstrap and the wheel tied to this install. It is written to
# <prefix>/python/wheelhouse so it can be consumed via pip --find-links.
#
# At runtime the bootstrap (which runs at EVERY interpreter startup) does the
# minimum, and never spawns a process or sources setup.sh:
#   1. resolve the install -- the baked prefix if its python/ dir still exists,
#      else $MDSPLUS_DIR;
#   2. set MDSPLUS_DIR to it (deterministically -- so the package and its C
#      libraries always resolve to the same install) for the library loader;
#   3. prepend its python/ dir to sys.path so this install wins.
#
# The wheel is assembled directly by make_wheel.py (standard library only) -- no
# build backend, no pip build isolation, no network access. Its dependencies are
# copied from python/MDSplus/pyproject.toml so they never drift.

set(_source_toml ${CMAKE_SOURCE_DIR}/python/MDSplus/pyproject.toml)
set(_builder ${CMAKE_CURRENT_SOURCE_DIR}/make_wheel.py)
set(_pth ${CMAKE_CURRENT_SOURCE_DIR}/MDSplus.pth)
set(_bootstrap_tmpl ${CMAKE_CURRENT_SOURCE_DIR}/_mdsplus_bootstrap.py.in)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gen_wheel.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/gen_wheel.cmake
    @ONLY
)

install(SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/gen_wheel.cmake)

# Ship the rationale/usage docs next to the wheel in the wheelhouse.
install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/README.md
    DESTINATION python/wheelhouse
)
