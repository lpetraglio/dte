CC ?= gcc
CFLAGS ?= -g -O2
LDFLAGS ?=
AWK = awk
VERSION = $(shell mk/version.sh 1.9.1)

WARNINGS = \
    -Wall -Wextra -Wformat -Wformat-security \
    -Wmissing-prototypes -Wstrict-prototypes \
    -Wold-style-definition -Wwrite-strings -Wundef -Wshadow \
    -Werror=div-by-zero -Werror=implicit-function-declaration \
    -Wno-sign-compare -Wno-pointer-sign

WARNINGS_EXTRA = \
    -Wformat-signedness -Wformat-truncation -Wformat-overflow \
    -Wstringop-truncation -Wstringop-overflow -Wshift-overflow=2 \
    -Wframe-larger-than=32768 -Wvla -Wcast-align -Wcast-align=strict

BUILTIN_SYNTAX_FILES ?= \
    awk c config css d diff docker dte gitcommit gitrebase go html \
    ini java javascript lisp lua mail make markdown meson nginx ninja \
    php python robotstxt roff ruby scheme sed sh sql tex texmfcnf tmux \
    vala xml xresources zig inc/c-comment inc/c-uchar

BUILTIN_CONFIGS = $(addprefix config/, \
    rc binding/default compiler/gcc compiler/go \
    $(addprefix color/, reset reset-basic default darkgray) \
    $(addprefix syntax/, $(BUILTIN_SYNTAX_FILES)) )

TEST_CONFIGS := $(addprefix test/data/, $(addsuffix .dterc, \
    env thai crlf pipe redo fuzz1 fuzz2 ))

util_objects := $(call prefix-obj, build/util/, \
    ascii exec hashset line-iter path ptr-array readfile string \
    strtonum unicode utf8 xmalloc xreadwrite xsnprintf )

command_objects := $(call prefix-obj, build/command/, \
    args env macro parse run serialize )

editorconfig_objects := $(call prefix-obj, build/editorconfig/, \
    editorconfig ini match )

encoding_objects := $(call prefix-obj, build/encoding/, \
    bom convert decoder encoder encoding )

syntax_objects := $(call prefix-obj, build/syntax/, \
    bitset color highlight state syntax )

terminal_objects := $(call prefix-obj, build/terminal/, \
    color ecma48 input key mode no-op output rxvt terminal terminfo \
    winsize xterm xterm-keys )

editor_objects := $(call prefix-obj, build/, \
    alias bind block block-iter buffer change cmdline commands \
    compiler completion config ctags debug edit editor error \
    file-history file-option filetype frame history indent \
    load-save lock main mode move msg options regexp \
    screen screen-cmdline screen-status screen-tabbar screen-view \
    search selection show spawn tag view window ) \
    $(command_objects) \
    $(editorconfig_objects) \
    $(encoding_objects) \
    $(syntax_objects) \
    $(terminal_objects) \
    $(util_objects)

test_objects := $(call prefix-obj, build/test/, \
    cmdline command config editorconfig encoding filetype main \
    syntax terminal test util )

feature_tests := $(addprefix build/feature/, $(addsuffix .h, \
    dup3 pipe2 fsync ioctl-winsize posix_madvise ))

all_objects := $(editor_objects) $(test_objects)
build_subdirs := $(filter-out build/, $(sort $(dir $(all_objects)))) build/feature/

editor_sources := $(patsubst build/%.o, src/%.c, $(editor_objects))
test_sources := $(patsubst build/test/%.o, test/%.c, $(test_objects))

ifdef WERROR
  WARNINGS += -Werror
endif

CWARNS = $(WARNINGS) $(foreach W,$(WARNINGS_EXTRA),$(call cc-option,$(W)))
CSTD = $(call cc-option,-std=gnu11,-std=gnu99)
$(call make-lazy,CWARNS)
$(call make-lazy,CSTD)

ifeq "$(KERNEL)" "Darwin"
  LDLIBS_ICONV = -liconv
else ifeq "$(OS)" "Cygwin"
  LDLIBS_ICONV = -liconv
  EXEC_SUFFIX = .exe
else ifeq "$(KERNEL)" "OpenBSD"
  LDLIBS_ICONV = -liconv
  BASIC_CFLAGS += -I/usr/local/include
  BASIC_LDFLAGS += -L/usr/local/lib
else ifeq "$(KERNEL)" "NetBSD"
  ifeq ($(shell expr "`uname -r`" : '[01]\.'),2)
    LDLIBS_ICONV = -liconv
  endif
  BASIC_CFLAGS += -I/usr/pkg/include
  BASIC_LDFLAGS += -L/usr/pkg/lib
endif

ifdef ICONV_DISABLE
  build/encoding/convert.o: BASIC_CFLAGS += -DICONV_DISABLE=1
else
  LDLIBS += $(LDLIBS_ICONV)
endif

ifdef TERMINFO_DISABLE
  build/terminal/terminfo.o: BASIC_CFLAGS += -DTERMINFO_DISABLE=1
else
  LDLIBS += $(or $(call pkg-libs, tinfo), $(call pkg-libs, ncurses), -lcurses)
endif

ifdef SANE_WCTYPE
  BASIC_CFLAGS += -DSANE_WCTYPE=1
endif

dte = dte$(EXEC_SUFFIX)
test = build/test/test$(EXEC_SUFFIX)

ifdef USE_SANITIZER
  SANITIZER_FLAGS := \
    -fsanitize=address,undefined -fsanitize-address-use-after-scope \
    -fno-sanitize-recover=all -fno-omit-frame-pointer -fno-common
  CC_SANITIZER_FLAGS := $(or \
    $(call cc-option,$(SANITIZER_FLAGS)), \
    $(warning USE_SANITIZER set but compiler doesn't support ASan/UBSan) )
  $(all_objects): BASIC_CFLAGS += $(CC_SANITIZER_FLAGS)
  $(dte) $(test): BASIC_LDFLAGS += $(CC_SANITIZER_FLAGS)
  export ASAN_OPTIONS=detect_leaks=1:detect_stack_use_after_return=1
  DEBUG = 3
else
  # 0: Disable debugging
  # 1: Enable BUG_ON() and light-weight sanity checks
  # 3: Enable expensive sanity checks
  DEBUG ?= 0
endif

ifeq "$(DEBUG)" "0"
  UNWIND = $(call cc-option,-fno-asynchronous-unwind-tables)
  $(call make-lazy,UNWIND)
endif

BASIC_CFLAGS += \
    $(CSTD) $(CWARNS) $(UNWIND) \
    -DDEBUG=$(DEBUG) \
    -D_FILE_OFFSET_BITS=64

$(all_objects): BASIC_CFLAGS += -Isrc

# If "make install*" with no other named targets
ifeq "" "$(filter-out install install-desktop-file,$(or $(MAKECMDGOALS),all))"
  OPTCHECK = :
else
  OPTCHECK = SILENT_BUILD='$(MAKE_S)' mk/optcheck.sh
endif

ifndef NO_DEPS
  ifneq '' '$(call cc-option,-MMD -MP -MF /dev/null)'
    $(all_objects): DEPFLAGS = -MMD -MP -MF $(patsubst %.o, %.mk, $@)
  else ifneq '' '$(call cc-option,-MD -MF /dev/null)'
    $(all_objects): DEPFLAGS = -MD -MF $(patsubst %.o, %.mk, $@)
  endif
  -include $(patsubst %.o, %.mk, $(all_objects))
endif

$(dte): $(editor_objects)
$(test): $(filter-out build/main.o, $(all_objects))
$(util_objects): | build/util/
$(command_objects): | build/command/
$(editorconfig_objects): | build/editorconfig/
$(encoding_objects): | build/encoding/
$(syntax_objects): | build/syntax/
$(terminal_objects): | build/terminal/
$(build_subdirs): | build/
$(feature_tests): mk/feature-test/defs.h build/all.cflags | build/feature/
build/builtin-config.h: build/builtin-config.mk
build/test/data.h: build/test/data.mk
build/config.o: build/builtin-config.h
build/test/config.o: build/test/data.h
build/editor.o: build/version.h
build/load-save.o: build/feature.h
build/util/exec.o: build/feature.h
build/terminal/winsize.o: build/feature.h
build/terminal/terminfo.o: build/terminal/terminfo.cflags
build/encoding/convert.o: build/encoding/convert.cflags
build/terminal/terminfo.cflags: | build/terminal/
build/encoding/convert.cflags: | build/encoding/

CFLAGS_ALL = $(CPPFLAGS) $(CFLAGS) $(BASIC_CFLAGS)
LDFLAGS_ALL = $(CFLAGS) $(LDFLAGS) $(BASIC_LDFLAGS)

$(dte) $(test): build/all.ldflags
	$(E) LINK $@
	$(Q) $(CC) $(LDFLAGS_ALL) -o $@ $(filter %.o, $^) $(LDLIBS)

$(editor_objects): build/%.o: src/%.c build/all.cflags | build/
	$(E) CC $@
	$(Q) $(CC) $(CFLAGS_ALL) $(DEPFLAGS) -c -o $@ $<

$(test_objects): build/test/%.o: test/%.c build/all.cflags | build/test/
	$(E) CC $@
	$(Q) $(CC) $(CFLAGS_ALL) $(DEPFLAGS) -c -o $@ $<

build/all.ldflags: FORCE | build/
	@$(OPTCHECK) '$(CC) $(LDFLAGS_ALL) $(LDLIBS)' $@

build/%.cflags: FORCE | build/
	@$(OPTCHECK) '$(CC) $(CFLAGS_ALL)' $@

build/version.h: FORCE | build/
	@$(OPTCHECK) 'static const char version[] = "$(VERSION)";' $@

build/builtin-config.mk: FORCE | build/
	@$(OPTCHECK) '$(@:.mk=.h): $(BUILTIN_CONFIGS)' $@

build/test/data.mk: FORCE | build/test/
	@$(OPTCHECK) '$(@:.mk=.h): $(TEST_CONFIGS)' $@

build/builtin-config.h: $(BUILTIN_CONFIGS) mk/config2c.awk | build/
	$(E) GEN $@
	$(Q) $(AWK) -f mk/config2c.awk $(BUILTIN_CONFIGS) > $@

build/test/data.h: $(TEST_CONFIGS) mk/config2c.awk | build/test/
	$(E) GEN $@
	$(Q) $(AWK) -f mk/config2c.awk $(TEST_CONFIGS) > $@

build/feature.h: mk/feature-test/defs.h $(feature_tests)
	$(E) GEN $@
	$(Q) cat $^ > $@

$(feature_tests): build/feature/%.h: mk/feature-test/%.c mk/feature-test/%.h
	$(E) DETECT $*
	$(Q) if $(CC) $(CFLAGS_ALL) -o $(@:.h=.o) $< 2>>$(@:.h=.log); then \
	  cp $(<:.c=.h) $@ ; \
	else \
	  echo > $@ ; \
	fi

build/ $(build_subdirs):
	$(Q) mkdir -p $@


CLEANFILES += $(dte)
CLEANDIRS += build/
.PHONY: FORCE
.SECONDARY: build/
