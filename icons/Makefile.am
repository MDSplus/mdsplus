.rc.o:
	@WINDRESwindres@ $^ -o $@
%.o : %.rc
	@WINDRES@ $^ -o $@

AM_CFLAGS = $(TARGET_ARCH)

bin_PROGRAMS = icons
icons_SOURCES = icons.rc main.c

#FIXME: Clean up LIBS in top level configure
LIBS=

