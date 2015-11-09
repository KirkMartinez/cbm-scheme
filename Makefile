#
# Makefile for cbm-scheme
#

# Enter the target system for cc65 here
SYS	= c128

# Location of cc65 binaries
CC65_HOME=/Users/kirkmartinez/git/cc65

CLIB = --lib $(SYS).lib
CL   = $(CC65_HOME)/bin/cl65
CC   = $(CC65_HOME)/bin/cc65
AS   = $(CC65_HOME)/bin/ca65
LD   = $(CC65_HOME)/bin/ld65

# VICE provides this for making disk images
C1541	= c1541

# --------------------------------------------------------------------------
# Generic rules

%: %.c
%: %.s

.c.o:
	@echo $<
	$(CC) $(CFLAGS) -Oirs --codesize 500 -T -g -t $(SYS) $<
	$(AS) $(<:.c=.s)

.s.o:
	@echo $<
	$(AS) $(AFLAGS) -t $(SYS) $<

.PRECIOUS: %.o

.o:
	$(LD) $(LDFLAGS_$(@F)_$(SYS)) -o $@ -t $(SYS) -m $@.map $^ $(CLIB) 

# --------------------------------------------------------------------------
# List of executables.

EXELIST	=	testrunner \
		scheme		

# --------------------------------------------------------------------------
# Rules to make the binaries

.PHONY:	all
all:	$(EXELIST)

scheme: getc_conio.o

testrunner: getc_conio.o

# --------------------------------------------------------------------------
# Make CBM disk. Needs the c1541 program that comes with the VICE emulator

.PHONY:	disk
disk:	scheme.d64

scheme.d64:	all
	@$(C1541) -format scheme,AA  d64 $@ > /dev/null
	@for exe in $(EXELIST); do\
	    $(C1541) -attach $@ -write $$exe > /dev/null || exit $$?;\
	done

# --------------------------------------------------------------------------
# Clean-up rules

.PHONY:	clean
clean:
	$(RM) *~ *.map *.o *.s *.lbl

.PHONY:	zap
zap:	clean
	$(RM) $(EXELIST) scheme.d64

