# Makefile to build class 'helloworld' for Pure Data.
# Needs Makefile.pdlibbuilder as helper makefile for platform-dependent build
# settings and rules.

# library name
lib.name = stp_comp~

# input source file (class name == source file basename)
stp_comp~.class.sources = stp_comp_pd.c stp_comp.c

# all extra files to be included in binary distribution of the library
datafiles = stp_comp-help.pd stp_comp-meta.pd README.md

# include Makefile.pdlibbuilder from submodule directory 'pd-lib-builder'
PDLIBBUILDER_DIR=pd-lib-builder/
include $(PDLIBBUILDER_DIR)/Makefile.pdlibbuilder
