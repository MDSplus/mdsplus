# ------------------------------------------------------------------------
# Makefile to drive the jScope-TWU testing codes.
# J.G. Krom, J.krom@fz-juelich.de
#
# This should probably be converted to automake&autoconf etc.. Also a
# more windows orientated mechanism would be welcome.  But this works
# for me for the moment.
#
# $Id$
# ------------------------------------------------------------------------

TEST1=TestTWUSignalProperties
TEST1CLASSES=TestTWUSignalProperties.class TWUSignal.class TWUProperties.class

TEST2=TestTWUFetchOptions
TEST2CLASSES=TestTWUFetchOptions.class     TWUFetchOptions.class

TEST3=TestTwuSingleSignal
TEST3CLASSES=TestTwuSingleSignal.class     TwuSingleSignal.class

TEST4=TestTwuSingleSignal2
TEST4CLASSES=TestTwuSingleSignal2.class    TwuSingleSignal.class

# ------------------------------------------------------------------------

all:	test1 test2 test3 test4

clean:
	rm -f *.class *~


# ------------------------------------------------------------------------

VPATH = ..:.

# JAVAVERBOSENESS=-g -O -verbose -deprecation
JAVAVERBOSENESS=-g -O -deprecation

%.class:%.java
	javac $(JAVAVERBOSENESS) -sourcepath .. -d . $<

%.class:../%.java
	javac $(JAVAVERBOSENESS) -sourcepath .. -d . $<

# ------------------------------------------------------------------------

test1:$(TEST1CLASSES)
	java $(TEST1)

test2:$(TEST2CLASSES)
	java $(TEST2)

test3:$(TEST3CLASSES)
	java $(TEST3)

test4:$(TEST4CLASSES)
	java $(TEST4)

# ------------------------------------------------------------------------
