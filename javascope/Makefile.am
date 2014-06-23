JAVAROOT = $(builddir)
EXTRA_DIST = $(MINDTERM)
CLEANFILES = $(bin_SCRIPTS) $(java_DATA)

MINDTERM = $(srcdir)/MindTerm.jar
CLASSPATH_ENV = CLASSPATH=$(MINDTERM)

# Create the shell script that runs the program
bin_SCRIPTS = scripts/jScope
EXTRA_DIST += jScope.template
$(bin_SCRIPTS): jScope.template
	$(INSTALL) -D $< $@

# Documentation
dist_docs_DATA = $(DOCS)
docsdir = $(exec_prefix)/java/classes/docs

dist_java_DATA = jScope.properties MindTerm.jar

java_DATA = jScope.jar WaveDisplay.jar
javadir = $(exec_prefix)/java/classes
$(java_DATA): classdist_java.stamp

EXTRA_DIST += colors1.tbl
jScope.jar: colors1.tbl
	$(JAR) c0f $@ $(JSCOPE_CLASS) $(dist_docs_DATA) $<

WaveDisplay.jar:
	$(JAR) c0f $@ $(WAVEDISPLAY_CLASS)

dist_java_JAVA = $(JSCOPE_SRC) $(WAVEDISPLAY_SRC) $(COMMON_SRC)
COMMON_CLASS = $(COMMON_SRC:.java=*.class)
JSCOPE_CLASS = $(COMMON_CLASS) $(JSCOPE_SRC:.java=*.class)
WAVEDISPLAY_CLASS = $(COMMON_CLASS) $(WAVEDISPLAY_SRC:.java=*.class)
CLEANFILES += $(JSCOPE_CLASS) $(WAVEDISPLAY_CLASS)

