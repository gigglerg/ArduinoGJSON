# log output file and doxygen configuration
DOXYLOG = ./doxygen.log
DOXYFILE = doxyfile

# get name of folder above ./docs, this is the project
PRJ_NAME = $(shell basename `realpath ..`)

# newline
define nl


endef

# get git tag, if no tag then get last commit, failing that leave it empty
GIT_STR=$(shell git tag -l --points-at HEAD 2>/dev/null)
ifeq ($(GIT_STR),)
	GIT_STR=$(shell git log --pretty=format:'%h' -n 1 2>/dev/null)
endif

ifeq ($(GIT_STR),)
	GIT_TAG=''
else
	GIT_TAG="$(GIT_STR) (git)"
endif

# default.  documentation for avr8 (mega) and all other architectures
all: examples.txt all_edit avr8_edit doxygen

# update project number with git details
.PHONY: all_edit
all_edit:
	@sed -i -E 's|(PROJECT_NAME[ \t]+)\=([ \t]+.*)$$|\1= ${PRJ_NAME}|' $(DOXYFILE)
	@sed -i -E 's|(PROJECT_NUMBER[ \t]+)\=([ \t]+.*)$$|\1= ${GIT_TAG}|' $(DOXYFILE)

# update build specific macros and output folder
.PHONY: avr8_edit
avr8_edit:
	@sed -i -E "s/^(PREDEFINED)([ ]+)\=([A-Za-z0-9\_ ]*)/PREDEFINED             \= ARDUINO_ARCH_AVR/" $(DOXYFILE)
	@sed -i -E "s/^(HTML_OUTPUT)([ ]+)\=([A-Za-z0-9\_ ]*)/HTML_OUTPUT            \= avr8_html/" $(DOXYFILE)

# general problem with doxygen and md files where the relative location isnt the same, make sure all are "../"
.PHONY: md_correct
md_correct:
	@python mdlinks.py -i ../README.md -e '\((?P<replace>[\w\_\-\d\.]+)(\/\w+)(\_html\/)([\w\.\d]*)\)' -r '..' -o

# generate doxygen documentation
.PHONY: doxygen
doxygen: md_correct
	$(info Generating documentation)
	@truncate $(DOXYLOG) -s 0
	@doxygen $(DOXYFILE)
	$(info Done. Doxygen Log:$(nl))
	@cat $(DOXYLOG)
    # general problem with doxygen and md files where the relative location isnt the same, make sure all are "docs/"
	@python mdlinks.py -i ../README.md -e '\((?P<replace>[\w\_\-\d\.]+)(\/\w+)(\_html\/)([\w\.\d]*)\)' -r 'docs' -o

# clean and tidy...
.PHONY: clean
clean:
	$(info Clean)
	@rm -rf avr8_html
	@rm -f $(DOXYLOG)
	@rm -f examples.txt

# look for example .ino files and include them by referencing in a \example and \file doxygen tag
# \file removed as not required, left as standard block comment, non-parsed
.PHONY: examples.txt
examples.txt:
	$(shell truncate -s 0 $@)
	$(eval example_files := $(wildcard ../examples/*/*.ino))
	$(foreach f,$(example_files), $(shell echo -e '/** \\example $(notdir $f) */ \n/* file $(notdir $f) */' >> $@))
