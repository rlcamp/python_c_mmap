# autogenerated
# overrideable vars used in implicit make rules
# only default to -march=native if not on an arm mac
ifeq (,$(findstring arm64,$(shell uname -m)))
TARGET_ARCH ?= -march=native
endif
CFLAGS ?= -Os
CPPFLAGS += -Wall -Wextra -Wshadow -Wmissing-prototypes
LDFLAGS += ${CFLAGS}

# list of targets to build, generated from .c files containing a main() function:

TARGETS=child

all : ${TARGETS}

# for each target, the list of objects to link, generated by recursively crawling include statements with a corresponding .c file:

child : child.o

# for each object, the list of headers it depends on, generated by recursively crawling include statements:

child.o :

*.o : Makefile

clean :
	$(RM) -rf *.o *.dSYM ${TARGETS}
.PHONY: clean all
