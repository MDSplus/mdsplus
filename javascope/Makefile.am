JAVAROOT = $(builddir)
EXTRA_DIST = $(MINDTERM)
CLEANFILES = $(java_DATA)

MINDTERM = $(srcdir)/MindTerm.jar
CLASSPATH_ENV = CLASSPATH=$(MINDTERM)

# Create the shell script that runs the program
if MINGW
bin_SCRIPTS = jScope.bat
else
bin_SCRIPTS = scripts/jScope
EXTRA_DIST += jScope.template
CLEANFILES += $(bin_SCRIPTS)

$(bin_SCRIPTS): jScope.template
	@ $(INSTALL) -d scripts; \
	  $(INSTALL) $< $@


clean-local:
	-rm -rf scripts

endif


# Documentation
dist_docs_DATA = $(DOCS)
docsdir = $(exec_prefix)/java/classes/jdocs

dist_java_DATA = jScope.properties MindTerm.jar

java_DATA = jScope.jar WaveDisplay.jar
javadir = $(exec_prefix)/java/classes
$(java_DATA): classjava.stamp

EXTRA_DIST += colors1.tbl
if USE_JARS_DIR
jScope.jar: @JARS_DIR@/javascope/jScope.jar
	cp $< $@

WaveDisplay.jar: @JARS_DIR@/javascope/WaveDisplay.jar
	cp $< $@

else
jScope.jar: colors1.tbl
	@ echo "Creating JAR: $@ ..."; \
	  $(MKDIR_P) $(builddir)/jdocs; \
	  cp $(addprefix $(srcdir)/,$(DOCS)) $(builddir)/jdocs; \
	  $(JAR) c0f $@ $(JSCOPE_CLASS) $(builddir)/jdocs \
		     -C $(srcdir) colors1.tbl; \
	  rm -Rf $(builddir)/jdocs;

WaveDisplay.jar:
	@ echo "Creating JAR: $@ ..."; \
	  $(JAR) c0f $@ $(WAVEDISPLAY_CLASS)

dist_java_JAVA = $(JSCOPE_SRC) $(WAVEDISPLAY_SRC) $(COMMON_SRC)
COMMON_CLASS = $(COMMON_SRC:.java=*.class)
JSCOPE_CLASS = $(COMMON_CLASS) $(JSCOPE_SRC:.java=*.class) $(EXTRA_CLASS)
WAVEDISPLAY_CLASS = $(COMMON_CLASS) $(WAVEDISPLAY_SRC:.java=*.class)
CLEANFILES += $(JSCOPE_CLASS) $(WAVEDISPLAY_CLASS) $(EXTRA_CLASS)
endif

include Makefile.common
