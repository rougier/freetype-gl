# =========================================================================
# Freetype GL - A C OpenGL Freetype engine
# Platform:    Any
# WWW:         http://code.google.com/p/freetype-gl/
# -------------------------------------------------------------------------
# Copyright 2011 Nicolas P. Rougier. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#  1. Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY NICOLAS P. ROUGIER ''AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
# EVENT SHALL NICOLAS P. ROUGIER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are
# those of the authors and should not be interpreted as representing official
# policies, either expressed or implied, of Nicolas P. Rougier.
# =========================================================================
PLATFORM		= $(shell uname)
CC				= gcc
CFLAGS			= -Wall `freetype-config --cflags` -I/usr/X11/include
LIBS			= -lGL -lglut -lGLU -lglew \
	              `freetype-config --libs` -lfontconfig
ifeq ($(PLATFORM), Darwin)
	LIBS		= -framework OpenGL -framework GLUT -lglew \
	               `freetype-config --libs` -L /usr/X11/lib -lfontconfig
endif

DEMOS     := $(patsubst %.c,%,$(wildcard demo-*.c))
DEMOS_ATB := $(patsubst %.c,%,$(wildcard demo-atb-*.c))
DEMOS     := $(filter-out $(DEMOS_ATB), $(DEMOS))

TESTS     := $(patsubst %.c,%,$(wildcard test-*.c))
HEADERS   := $(wildcard *.h)
SOURCES   := $(filter-out $(wildcard demo-*.c), $(wildcard *.c))
SOURCES   := $(filter-out $(wildcard test-*.c), $(SOURCES))
SOURCES   := $(filter-out makefont.c, $(SOURCES))
OBJECTS   := $(SOURCES:.c=.o)

.PHONY: all clean distclean
all: $(DEMOS) makefont demo-atb-agg $(TESTS)

demos: $(DEMOS)

tests: $(TESTS)

define DEMO_template
$(1): $(1).o $(OBJECTS) $(HEADERS)
	@echo "Building $$@... "
	@$(CC) $(OBJECTS) $(1).o $(LIBS) -o $$@
endef
$(foreach demo,$(DEMOS),$(eval $(call DEMO_template,$(demo))))

define TEST_template
$(1): $(1).o $(OBJECTS) $(HEADERS)
	@echo "Building $$@... "
	@$(CC) $(OBJECTS) $(1).o $(LIBS) -o $$@
endef
$(foreach test,$(TESTS),$(eval $(call TEST_template,$(test))))

%.o : %.c
	@echo "Building $@... "
	@$(CC) -c $(CFLAGS) $< -o $@ 


demo-atb-agg: demo-atb-agg.o $(OBJECTS) $(HEADERS) \
	          Arial.ttf Tahoma.ttf Verdana.ttf Times.ttf Georgia.ttf
	@echo "Building $@... "
	@$(CC) $(OBJECTS) $@.o $(LIBS) -lAntTweakBar -o $@


makefont: makefont.o $(OBJECTS) $(HEADERS)
	@echo "Building $@... "
	@$(CC) $(OBJECTS) $@.o $(LIBS) -o $@

clean:
	@-rm -f $(DEMOS) $(DEMOS_ATB) *.o
	@-rm -f $(TESTS) *.o

distclean: clean
	@-rm -f *~
