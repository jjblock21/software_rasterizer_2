NAME = software_rasterizer

COMPILER = clang -c
RFLAGS = -std=c17 -O3 -Iinclude
DFLAGS = -std=c17 -O0 -Iinclude

LINKER = clang
LFLAGS =

SRCDIR = src
OUTDIR_RELEASE = bin\release
OUTDIR_DEBUG = bin\debug

# Sources to include in the executable
SOURCES := $(subst $(shell cd)\,,								\
			  $(shell dir "$(SRCDIR)\" /s /b | findstr \S*\.c)	\
		  )														\

# Libraries to include in the executable
LIBS = lib\SDL2.lib lib\SDL2main.lib lib\cglm.lib

# Dependencies to copy to the output directory
DEPS = lib\SDL2.dll lib\cglm.dll

.PHONY: build debug clean run

build: FLAGS = $(RFLAGS)
build: $(OUTDIR_RELEASE) $(OUTDIR_RELEASE)\$(NAME).exe

debug: FLAGS = $(DFLAGS)
debug: $(OUTDIR_DEBUG) $(OUTDIR_DEBUG)\$(NAME).exe

clean:
	-rmdir /s /q $(OUTDIR_RELEASE)
	-rmdir /s /q $(OUTDIR_DEBUG)

run: build
	-$(OUTDIR_RELEASE)\$(NAME).exe

%.exe: $(SOURCES:.c=.o)
	$(LINKER) $(LFLAGS) $(SOURCES:.c=.o) $(LIBS) -o $@

%.o: %.c
	$(COMPILER) $(FLAGS) $< -o $@

# Create output directories and copy dependencies
$(OUTDIR_RELEASE) $(OUTDIR_DEBUG): $(DEPS)
	mkdir $@
	$(foreach DEP,$?,copy /y $(DEP) $@ &)