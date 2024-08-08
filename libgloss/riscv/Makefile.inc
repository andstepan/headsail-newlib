multilibtool_DATA += \
	%D%/nano.specs \
	%D%/sim.specs \
	%D%/crt0.o

multilibtool_LIBRARIES += %D%/libgloss.a
%C%_libgloss_a_CPPFLAGS = -I$(srcdir)/%D%
%C%_libgloss_a_SOURCES = \
	%D%/syscalls.c \
	%D%/uart8250.c

multilibtool_LIBRARIES += %D%/libsim.a
%C%_libsim_a_CPPFLAGS = $(%C%_libgloss_a_CPPFLAGS) -DUSING_NANO_SPECS
%C%_libsim_a_SOURCES = $(%C%_libgloss_a_SOURCES)

includemachinetooldir = $(tooldir)/include/machine
includemachinetool_DATA = %D%/machine/syscall.h
