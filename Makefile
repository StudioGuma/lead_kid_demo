#
# A Makefile that compiles all .c and .s files in "src" and "res"
# subdirectories and places the output in a "obj" subdirectory
#

# If you move this project you can change the directory
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
GBDK_HOME = ~/gbdk

LCC = ${GBDK_HOME}/bin/lcc

# You can set flags for LCC here
LCCFLAGS += -autobank -Iinclude -Wf--Werror -Wl-llib/hUGEDriver.lib \
-Wm-yn"LEAD KID DEMO" -Wm-yt0x19 -Wf-opt-code-speed

# DEBUG = ON
ifdef DEBUG
	LCCFLAGS += -debug -Wb-ext=.rel
endif

# You can set the name of the .gb ROM file here
PROJECTNAME = lead_kid_demo

SRCDIR      = src
OBJDIR      = obj
RESDIR      = res
BINS	    = $(OBJDIR)/$(PROJECTNAME).gb
CSOURCES    = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c))) $(foreach dir,$(RESDIR),$(notdir $(wildcard $(dir)/*.c)))
ASMSOURCES  = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.s)))
OBJS        = $(CSOURCES:%.c=$(OBJDIR)/%.o) $(ASMSOURCES:%.s=$(OBJDIR)/%.o)

all:	prepare $(BINS)

# Compile .c files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .s assembly files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.s
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Compile .c files in "res/" to .o object files
$(OBJDIR)/%.o:	$(RESDIR)/%.c
	$(LCC) $(LCCFLAGS) -c -o $@ $<

# Link the compiled object files into a .gb ROM file
$(BINS):	$(OBJS)
	$(LCC) $(LCCFLAGS) -o $(BINS) $(OBJS)

	$(GBDK_HOME)/bin/romusage $(OBJDIR)/$(PROJECTNAME).ihx

prepare:
	mkdir -p $(OBJDIR)

clean:
	rm -r $(OBJDIR)
