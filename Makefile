# List all files in the specified directory with the specified extension
__list_files = 																			 \
	$(subst \,/, 																		 \
		$(subst $(shell cd)\$(1)\,, 													 \
			$(shell dir "$(1)\" /s /b | findstr \S*\$(2)) 								 \
		) 																				 \
	)

# -------- DO NOT EDIT ABOVE THIS LINE --------

NAME = software_rasterizer

COMPILER = clang -c
CFLAGS = -std=c17 -O3 -Wall -Iinclude

LINKER = clang
LFLAGS = 

SRCDIR = src
OBJDIR = obj
OUTDIR = bin

SOURCES := $(call __list_files,$(SRCDIR),.c)
LIBS = lib/SDL2.lib lib/SDL2main.lib lib/cglm.lib
DEPS = lib/SDL2.dll lib/cglm.dll

# -------- DO NOT EDIT BELOW THIS LINE --------

OBJECTS := $(addprefix $(OBJDIR)/,$(patsubst %.c,%.o,$(SOURCES)))
DIRS := $(SRCDIR) $(patsubst %/,%,$(sort $(dir $(OBJECTS)))) $(OUTDIR)
OUTPATH := $(OUTDIR)/$(NAME).exe

.PHONY: build run clean rebuild
.FORCE = $(DIRS)

build: $(OUTPATH)

# Create output file and copy dependecies to the output dir
$(OUTPATH): $(DIRS) $(OBJECTS) $(DEPS)
	$(LINKER) $(LFLAGS) $(OBJECTS) $(LIBS) -o $(OUTPATH)

	$(foreach DEP,$(DEPS),																 \
		copy /y $(subst /,\,$(DEP)) $(subst /,\,$(OUTDIR)/$(notdir $(DEP))) &			 \
	)

# Create directories required for building
$(DIRS):
	-mkdir $(subst /,\,$@)

# Create object files from source files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(COMPILER) $(CFLAGS) $< -o $@

run: build
	-$(OUTPATH)

# Delete the obj dir and output dir
clean:
	-rmdir /s /q $(OUTDIR)
	-rmdir /s /q $(OBJDIR)

rebuild: clean build