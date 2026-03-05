MAKE_FLAGS += -j

BIN_NAME := nanotekspice

SRC := $(wildcard src/*.cpp)
SRC += $(wildcard src/*/*.cpp)

BUILD_DIR := .build

CFLAGS += -Wall -Wextra -Werror=write-strings
CFLAGS += -Wno-unused-parameter -Wunused-result
CFLAGS += -Wp,-U_FORTIFY_SOURCE -Wcast-qual
CFLAGS += -Wformat=2 -Wshadow -fno-builtin
CFLAGS += -Wstrict-aliasing=0 -Wunreachable-code
CFLAGS += -Wwrite-strings -Werror=format-nonliteral -Werror=return-type
CFLAGS += -std=c++20 -iquote src

include utils.mk

.PHONY: _start all
_start: all

CXX = clang++

# call mk-profile release, SRC, additional CFLAGS
define mk-profile

NAME_$(strip $1) := $4
OBJ_$(strip $1) := $$($(strip $2):%.cpp=$$(BUILD_DIR)/$(strip $1)/%.o)

$$(BUILD_DIR)/$(strip $1)/%.o: %.cpp
	@ mkdir -p $$(dir $$@)
	@ $$(COMPILE.cpp) $$(CFLAGS) $$< -o $$@
	@ $$(LOG_TIME) "$$(C_GREEN) CC $$(C_PURPLE) $$(notdir $$@) $$(C_RESET)"

$$(NAME_$(strip $1)): $$(OBJ_$(strip $1))
	@ $$(LINK.cpp) $$(OBJ_$(strip $1)) $$(LDFLAGS) $$(LDLIBS) -o $$@
	@ $$(LOG_TIME) "$$(C_BLUE) LD $$(C_PURPLE) $$(notdir $$@) $$(C_RESET)"
	@ $$(LOG_TIME) "$$(C_GREEN) OK  Compilation finished $$(C_RESET)"

endef

$(eval $(call mk-profile, release, SRC, , $(BIN_NAME)))
$(eval $(call mk-profile, debug, SRC, -D DEBUG_MODE -lasan -fanalyzer -g3, debug))
$(eval $(call mk-profile, test, SRC, , test))

all: $(NAME_release)

tests: $(NAME_test)
	@ bash tests/run_all.sh

format:
	@ find src/ -name "*.cpp" -type f -exec clang-format -i {} ";"
	@ find src/ -name "*.hpp" -type f -exec clang-format -i {} ";"
	@ find tests/ -name "*.cpp" -type f -exec clang-format -i {} ";"
	@ find tests/ -name "*.hpp" -type f -exec clang-format -i {} ";"
	@ $(LOG_TIME) "$(C_BLUE) CF $(C_GREEN) Code formated  $(C_RESET)"

clean:
	@ $(RM) $(OBJ)
	@ $(LOG_TIME) "$(C_YELLOW) RM $(C_PURPLE) $(OBJ) $(C_RESET)"

fclean:
	@ $(RM) -r $(NAME_release) $(NAME_debug) $(BUILD_DIR)
	@ $(LOG_TIME) "$(C_YELLOW) RM $(C_PURPLE) $(NAME_release) $(NAME_debug) \
		$(C_RESET)"

.NOTPARALLEL: re
re:	fclean all

.PHONY: all clean fclean re

PREFIX ?=
BINDIR ?= $(PREFIX)/bin

.PHONY: install uninstall
install: $(NAME_release)
	install -Dm755 -t $(BINDIR) $(NAME_release)

uninstall:
	$(RM) $(BINDIR)/$(NAME_release)
