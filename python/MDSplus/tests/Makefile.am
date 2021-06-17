
include @top_builddir@/Makefile.inc
include ../../../testing/testing.am


## ////////////////////////////////////////////////////////////////////////// ##
## // TESTS  //////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##

TEST_EXTENSIONS = .py

TEST_FLAGS = @MAKEFLAG_J@

TESTS = \
data_data_test.py \
data_scalars_test.py \
data_arrays_test.py \
data_vms_test.py \
data_tdi_test.py \
data_decompile_test.py \
data_casts_test.py \
data_tdipy_test.py \
devices_mit_test.py \
devices_rfx_test.py \
devices_w7x_test.py \
exception_default_test.py \
exception_custom_test.py \
exception_tcl_test.py \
exception_segfault_test.py \
segment_dim_order_test.py \
segment_block_rows_test.py \
segment_write_test.py \
segment_opaque_test.py \
segment_update_test.py \
segment_time_context_test.py \
segment_scaled_test.py \
segment_dimless_test.py \
segment_compress_test.py \
segment_compress_ts_test.py \
task_do_test.py \
tree_attr_test.py \
tree_open_test.py \
tree_node_test.py \
tree_find_test.py \
tree_default_test.py \
tree_linkage_test.py \
tree_nci_test.py \
tree_data_test.py \
tree_xysignal_test.py \
tree_compression_test.py \
tree_records_test.py \
connection_io_test.py \
connection_thick_test.py \
connection_thread_test.py \
connection_tunnel_test.py \
connection_tcp_test.py \
connection_write_test.py \
dcl_interface_test.py
if !MINGW
TESTS += dcl_dispatcher_test.py
endif

# connection_write_test.py causes valgrind to hang?
VALGRIND_TESTS = $(filter-out connection_write_test.py,$(TESTS))

VALGRIND_SUPPRESSIONS_FILES =

# Skipping child programs that are launched inside python classes
#
VALGRIND_FLAGS = --trace-children-skip=*/ld,*/collect2,*/ldconfig,*/sh

# Files produced by tests that must be purged
#
MOSTLYCLEANFILES = *.log *.trs

## ////////////////////////////////////////////////////////////////////////// ##
## // TARGETS  ////////////////////////////////////////////////////////////// ##
## ////////////////////////////////////////////////////////////////////////// ##


clean-local: clean-local-tests

check_PROGRAMS =
check_SCRIPTS  = $(TESTS)
