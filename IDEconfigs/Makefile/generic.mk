#
# Copyright (C) 2009 Alexander Kl"aser
# 
# This piece is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
# 
# This software has been downloaded from:
# http://lear.inrialpes.fr/people/klaeser/software
# 
#
# Variables that need to be set in the Makefile that includes this file:
#   TARGETS   all files that are exectuables without there .cpp extension
#   BUILDDIR  temporary dir where things are compiled to (optional, by default ".build")
#   BINDIR    dir where executables are linked to (optional, by default "bin")
#   SRCDIRS   list of directories in which source files are located
#             this variable needs to be set if you do not have your source and
#             include files located in the same directory!
#
# Variables used for compiling/linking:
#   CXXFLAGS  flags for compiling
#   LDFLAGS   flags used for linking
#   LDLIBS    list of libraries to be linked
#   CXX       compiler linker (should be g++ by default)
#

# set paths for the dependency tool and gcc
DEP = IDEconfigs/Makefile/dep.py

# set some standard directories in case they have not been set
BUILDDIR ?= .build
BINDIR ?= bin

# all include files
INCLUDES := $(addprefix $(BUILDDIR)/,$(TARGETS:=.l))


#
# some general rules
#

.PHONY: all clean
.PRECIOUS: $(BUILDDIR)/%.d

all: $(BINDIR) $(addprefix $(BINDIR)/,$(TARGETS))

$(INCLUDES): $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR) $(addprefix $(BINDIR)/,$(TARGETS))

$(BUILDDIR) $(BINDIR):
	@mkdir -p $@ 


#
# rules for creating dependency files
#

# dependencies of .cpp files on other files
$(BUILDDIR)/%.d: %.cpp
	@echo CDep:$*.cpp
	@test -e $(dir $@) || mkdir -p $(dir $@)
	@g++ $(CXXFLAGS) -MM -MT $(BUILDDIR)/$*.o -MT $(BUILDDIR)/$*.d -MF $@ $< 

$(BUILDDIR)/%.d: %.cxx
	@echo CDep:$*.cxx
	@test -e $(dir $@) || mkdir -p $(dir $@)
	@g++ $(CXXFLAGS) -MM -MT $(BUILDDIR)/$*.o -MT $(BUILDDIR)/$*.d -MF $@ $< 

# dependencies for the linking
%.l: %.d
	@echo LDep:$*
	@test -e $(dir $@) || mkdir -p $(dir $@)
	@$(DEP) "$(@:$(BUILDDIR)/%.l=$(BINDIR)/%)" $@ $(BUILDDIR) $< $(SRCDIRS) > $@ 


#
# rules for compiling and linking
#

# compiling
$(BUILDDIR)/%.o: %.cpp
	@echo Comp:$*.cpp
	@test -e $(dir $@) || mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILDDIR)/%.o: %.cxx
	@echo Comp:$*.cxx
	@test -e $(dir $@) || mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c -o $@ $< 

# linking .. link dependencies are defined in the .l files
$(BINDIR)/%:
	@echo Link:$*
	@rm -f $@
	@test -e $(dir $@) || mkdir -p $(dir $@)
	@$(CXX) $(LDFLAGS) -o $@ $(filter %.o, $^) $(LDLIBS) 

%: %.o
%.h: ;
%.hpp: ;
%.c: ;
%.cpp: ;
%.cxx: ;


#
# include dependency files
#

ifneq ($(MAKECMDGOALS),clean)
-include $(INCLUDES)
endif
