#
# Makefile for KataDictionaryReplacer exercise
#  http://codingdojo.org/cgi-bin/index.pl?KataDictionaryReplacer
#
# Written in C language by Ismo Suihko, Espoo Finland, 2017-01-01
#
# Developed and tested using GNU C compiler in Fedora Linux
# 

CC=gcc
CFLAGS=

#
# CDEFS
#
# Could experiment e.g. with small output text buffer sizes
#  -DMAX_OUTPUT_STRING=nnn
#
# Also could set the max number of dictionary entries
#  -DMAX_DICT_KEYS=nnn
#
# Both have default values set in kdr.h and this overrides.
#
CDEFS=
#CDEFS=-DMAX_OUTPUT_STRING=200
#CDEFS=-DMAX_OUTPUT_STRING=60 -DMAX_DICT_KEYS=10
#CDEFS=-DMAX_OUTPUT_STRING=128 -DMAX_DICT_KEYS=100

OBJ=kdr_main.o kdr_build_dictionary.o kdr_process_input.o

TARFILE=KDR_IsmoSuihko

%.o: %.c kdr.h Makefile
        $(CC) -c -o $@ $< $(CFLAGS) $(CDEFS)

kdr: $(OBJ)
        $(CC) -o $@ $^ $(CLAGS)

clean:
        rm -f $(OBJ) core
        strip kdr

pack:
        tar cvf $(TARFILE).tar Makefile *.c *.h README tests
        gzip $(TARFILE).tar
