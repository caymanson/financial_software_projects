# makefile to build library and testdriver
.SUFFIXES:.cc
.cc.o:
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $@

# assign your compiler name to CC if not g++
CC = g++
# where your header files are (. is same directory as the makefile)
INCLUDES=-I.
# turn on debugging symbols
CFLAGS=-g
# where your object files are (. is the same directory as the makefile)
LDFLAGS=-L.

# when "make" is typed at command line with no args "default" will get executed and do "all"
default: 	all

#
# assign source and headers for our "utility" lib to variables below
#
# replace "SBB" (my initials) with yours or use the same filenames
LIB_UTIL_SRC = SBB_util.cc
LIB_UTIL_SRC_HDR = SBB_util.h
LIB_IO_SRC = SBB_io.cc
LIB_IO_HDR = SBB_io.h
LIB_DATE_SRC = SBB_date.cc
LIB_DATE_HDR = SBB_date.h
LIB_RATINGS_SRC = SBB_ratings.cc
LIB_RATINGS_HDR = SBB_ratings.h
LIB_CALCULATOR_SRC = BondCalculator.cc
LIB_CALCULATOR_HDR = BondCalculator.h
LIB_PORTFOLIO_SRC = Portfolio.cc
LIB_PORTFOLIO_HDR = Portfolio.h
#
LIB_UTIL_OBJ = $(LIB_UTIL_SRC:.cc=.o) $(LIB_IO_SRC:.cc=.o) $(LIB_DATE_SRC:.cc=.o) $(LIB_RATINGS_SRC:.cc=.o) $(LIB_CALCULATOR_SRC:.cc=.o) $(LIB_PORTFOLIO_SRC:.cc=.o)
$(LIB_UTIL_OBJ) : $(LIB_UTIL_SRC_HDR) $(LIB_IO_HDR) $(LIB_DATE_HDR) $(LIB_RATINGS_HDR) $(LIB_CALCULATOR_HDR) $(LIB_PORTFOLIO_HDR)
LIB_UTIL_TARGET = libutil.a

# 
# the file where main() is
MAIN_SRC = mmain.cc
MAINOBJ = $(MAIN_SRC:.cc=.o) 
$(MAINOBJ) : $(LIB_TARGET) $(LIB_UTIL_TARGET)
MAINTARGET=testdriver

ARCHIVE_OPTIONS = rucsv

$(LIB_UTIL_TARGET) : $(LIB_UTIL_OBJ)
	ar $(ARCHIVE_OPTIONS) $(LIB_UTIL_TARGET) $(LIB_UTIL_OBJ)

$(MAINTARGET) : $(MAINOBJ) $(LIB_TARGET) $(LIB_UTIL_TARGET)
	$(CC) $(LDFLAGS) $(MAINOBJ) $(LIB_UTIL_TARGET) $(LIB_TARGET) -o $(MAINTARGET)

all : util test

test: $(MAINTARGET)

util: $(LIB_UTIL_TARGET)


clean:
	rm -f *.o 

clobber:
	-rm -f *.o *.a $(MAINTARGET)

backup: clobber
	tar cvf Dongqiao_Wang.tar *.*
