# =========================================================================
# Freetype GL - A C OpenGL Freetype engine
# Platform:    Any
# WWW:         http://code.google.com/p/freetype-gl/
# -------------------------------------------------------------------------
# Copyright 2011,2012,2013 Nicolas P. Rougier. All rights reserved.
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
CFLAGS			= -Wno-deprecated-declarations -Wall `freetype-config --cflags` \
	              -I/usr/X11/include -O3 -I/opt/X11/include
LIBS			= -lGLEW -lGL -lglut -lGLU -lm \
	              `freetype-config --libs`
ifeq ($(PLATFORM), Darwin)
	LIBS		= -framework OpenGL -framework GLUT -lGLEW -lm \
	               `freetype-config --libs` -L/usr/X11/lib -L/opt/X11/lib
endif

DEMOS     := $(patsubst %.c,%,$(wildcard demo-*.c))
DEMOS_ATB := demo-atb-agg
DEMOS_MKP  := demo-markup
DEMOS     := $(filter-out $(DEMOS_ATB), $(DEMOS))
DEMOS     := $(filter-out $(DEMOS_MKP), $(DEMOS))

TESTS     := $(patsubst %.c,%,$(wildcard test-*.c))
HEADERS   := $(wildcard *.h)
SOURCES   := $(filter-out $(wildcard demo-*.c), $(wildcard *.c))
SOURCES   := $(filter-out makefont.c, $(SOURCES))
OBJECTS   := $(SOURCES:.c=.o)

.PHONY: all clean distclean
all: $(DEMOS) makefont demo-atb-agg demo-markup

demos: $(DEMOS)

tests: $(TESTS)

define DEMO_template
$(1): $(1).o $(OBJECTS) $(HEADERS)
	@echo "Building $$@... "
	@$(CC) $(OBJECTS) $(1).o $(LIBS) -o $$@
endef
$(foreach demo,$(DEMOS),$(eval $(call DEMO_template,$(demo))))

%.o : %.c
	@echo "Building $@... "
	@$(CC) -c $(CFLAGS) $< -o $@ 

demo-atb-agg: demo-atb-agg.o $(OBJECTS) $(HEADERS) \
	          fonts/Arial.ttf fonts/Tahoma.ttf fonts/Verdana.ttf fonts/Times.ttf fonts/Georgia.ttf
	@echo "Building $@... "
	@$(CC) $(OBJECTS) $@.o $(LIBS) -lAntTweakBar -o $@

demo-markup: demo-markup.o $(OBJECTS) $(HEADERS)
	@echo "Building $@... "
	@$(CC) $(OBJECTS) $@.o $(LIBS) -lfontconfig -o $@

makefont: makefont.o $(OBJECTS) $(HEADERS)
	@echo "Building $@... "
	@$(CC) $(OBJECTS) $@.o $(LIBS) -o $@

clean:
	@-rm -f $(DEMOS) $(DEMOS_ATB) $(DEMOS_MKP) makefont *.o
	@-rm -f $(TESTS) *.o

distclean: clean
	@-rm -f *~
