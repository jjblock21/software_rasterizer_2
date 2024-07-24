NAME = software_rasterizer

COMPILER = clang -c
CFLAGS_R = -std=c17 -Iinclude
CFLAGS_D = -std=c17 -g -Iinclude

LINKER = clang
LFLAGS =

SRCDIR = src
OBJDIR_R = obj/release
OBJDIR_D = obj/debug
OUTDIR_R = bin/release
OUTDIR_D = bin/debug

# Sources to include in the executable
# This automtically lists all .c files in SRCDIR
SOURCES := $(subst \,/,$(subst $(shell cd)\$(SRCDIR)\,,			\
			   $(shell dir "$(SRCDIR)\" /s /b | findstr \S*\.c)	\
		   ))

# Libraries to include in the executable
LIBS = lib\SDL2.lib lib\SDL2main.lib lib\cglm.lib

# Dependencies to create before building
DEPS = SDL2.dll cglm.dll

%/SDL2.dll:
	copy /y lib\SDl2.dll $(subst /,\,$@)

%/cglm.dll:
	copy /y lib\cglm.dll $(subst /,\,$@)

EXEC_R := $(OUTDIR_R)/$(NAME).exe
EXEC_D := $(OUTDIR_D)/$(NAME).exe
OBJECTS_R := $(addprefix $(OBJDIR_R)/,$(SOURCES:.c=.o))
OBJECTS_D := $(addprefix $(OBJDIR_D)/,$(SOURCES:.c=.o))
DIRS_R := $(OUTDIR_R) $(sort $(patsubst %/,%,$(dir $(OBJECTS_R))))
DIRS_D := $(OUTDIR_D) $(sort $(patsubst %/,%,$(dir $(OBJECTS_D))))
DEPS_R := $(addprefix $(OUTDIR_R)/,$(DEPS))
DEPS_D := $(addprefix $(OUTDIR_D)/,$(DEPS))
TO_CLEAN := $(wildcard $(OUTDIR_R) $(OUTDIR_D) $(OBJDIR_R) $(OBJDIR_D))

.PHONY: build_release build_debug deps run clean
.DEFAULT_GOAL := build_release

build_release: $(EXEC_R)
build_debug: $(EXEC_D)

run: build_release
	-$(EXEC_R)

clean:
ifneq ($(TO_CLEAN),)
	rmdir /s /q $(subst /,\,$(TO_CLEAN))
endif

# Build release executable
$(EXEC_R): CFLAGS = $(CFLAGS_R)
$(EXEC_R): $(DIRS_R) $(DEPS_R) $(OBJECTS_R)
	$(LINKER) $(LFLAGS) $(OBJECTS_R) $(LIBS) -o $(EXEC_R)

# Build debug executable
$(EXEC_D): CFLAGS = $(CFLAGS_D)
$(EXEC_D): $(DIRS_D) $(DEPS_D) $(OBJECTS_D)
	$(LINKER) $(LFLAGS) $(OBJECTS_D) $(LIBS) -o $(EXEC_D)

# Compile object files from source files
$(OBJDIR_R)/%.o $(OBJDIR_D)/%.o: $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $< -o $@

# Create directories required for build
$(DIRS_R) $(DIRS_D):
	mkdir $(subst /,\,$@)