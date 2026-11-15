VIMFORMAT_BIN       =  ./build/src/vim-format
GTEST_BIN           =  ./build/test/unittests
INSTALL_DIR         =  $$HOME/.local/bin
BUILD_DIR           =  $$PWD/build
SYMLINK_INSTALL     ?= 0

# Tip 6-1: Makefile logic at top maintains tab-completion
#
# Putting Makefile logic (ifeq, else, etc.) between different rules can mess up tab
# completion. Put it at the top to ensure all rules are visible by tab completion.

# Tip 7-1: Makefile assignment operators
#
#   =   Recursive expanded assignment
#
#       Evaluated only when the variable is referenced, not at the time it is defined.
#       Its definition can refer to variables not yet defined.
#
#   :=  Simple assignment
#
#       Behaves like traditional assignment. Evaluated once when it is defined.
#
#   ?=  Conditional assignment
#
#       Assigns only if the variable has not already been defined.

# Tip 8-1: Makefile CLI variable placement
#
#   Before (eg, 'DEBUG=1 make build')
#
#       This method sets DEBUG in the shell environment, then 'make' imports it along
#       with all other variables from the shell. Only applies to this invocation of
#       'make', not subsequent $(MAKE) calls.
#
#       Only ?= assignments inherit the CLI value. Assignment with = or := in the
#       Makefile override the CLI value.
#
#   After (eg, 'make build DEBUG=1') (Recommended)
#
#       This method sets DEBUG as a command-line argument to 'make'. Applies to the
#       top-level Makefile and exports down to sub-make commands triggered with $(MAKE).
#
#       All assignments (=, :=, ?=) in the Makefile are overridden by the CLI value.

# Tip 9-1: MAKEFLAGS env var
#
#   The MAKEFLAGS env var is a special variable managed by 'make'. It can be defined on
#   the CLI before or after the 'make' command and is automatically passed to sub-make
#   commands using $(MAKE).
#
#   Generally, do NOT define a MAKEFLAGS variable in the Makefile itself, except for rare
#   cases (eg, 'MAKEFLAGS += --no-print-directory').

ifeq ($(SYMLINK_INSTALL),0)
	INSTALL_CMD = cp
else ifeq ($(SYMLINK_INSTALL),1)
	INSTALL_CMD = ln -sfn
else
    $(error Expected 0 or 1 for SYMLINK_INSTALL, got $(SYMLINK_INSTALL))
endif


.PHONY: first
first: build


.PHONY: build

BUILD_TYPE := Release	# Release, Debug

build:
	cmake -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE)
	cmake --build $(BUILD_DIR)


.PHONY: install

install: build
	$(INSTALL_CMD) $(BUILD_DIR)/src/vim-format $(INSTALL_DIR)


.PHONY: test
test: build
	ctest --output-on-failure --test-dir build/test


.PHONY: clean
clean:
	rm -rf build/


.PHONY: format
format:
	find src/ test/ -type f \( -name '*.h' -or -name '*.cpp' \) -exec clang-format -i {} +


.PHONY: format/check
format/check:
	find src/ test/ -type f \( -name '*.h' -or -name '*.cpp' \) -exec clang-format --dry-run -Werror {} +


.PHONY: tags
tags:
	./scripts/cmds.py > ./reference/ex_cmds.txt


.PHONY: ast

VIM_FILES := $(wildcard test/ast/*.vim)
AST_FILES := $(VIM_FILES:.vim=.ast)

ast: $(AST_FILES)

test/ast/%.ast: test/ast/%.vim
	$(VIMFORMAT_BIN) -p $< > $@


.PHONY: errors

TEST_ERR_INFILES := $(wildcard test/error/*.vim)
TEST_ERR_OUTFILES := $(TEST_ERR_INFILES:.vim=.out)

errors: $(TEST_ERR_OUTFILES)


# Append '|| true' to ensure the line evaluates to a zero exit status and doesn't crash
# the Makefile.
test/error/%.out: test/error/%.vim
	$(VIMFORMAT_BIN) $< 2> $@ || true


.PHONY: pretty

TEST_PRETTY_INFILES := $(wildcard test/pretty/*.vim)
TEST_PRETTY_OUTFILES := $(TEST_PRETTY_INFILES:.vim=.pretty)

pretty: $(TEST_PRETTY_OUTFILES)

test/pretty/%.pretty: test/pretty/%.vim
	$(VIMFORMAT_BIN) $< > $@
