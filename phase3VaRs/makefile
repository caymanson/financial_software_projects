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
#
LIB_DATE_SRC = SBB_date.cc
LIB_DATE_SRC_HDR = SBB_date.h
#
LIB_IO_SRC = io.cc
LIB_IO_SRC_HDR = io.h
#
LIB_UTIL_OBJ = $(LIB_UTIL_SRC:.cc=.o) $(LIB_DATE_SRC:.cc=.o) $(LIB_IO_SRC:.cc=.o)
$(LIB_UTIL_OBJ) : $(LIB_UTIL_SRC_HDR) $(LIB_DATE_SRC_HDR) $(LIB_IO_SRC_HDR)
LIB_UTIL_TARGET = libutil.a
#
LIB_RATING_SRC = SBB_ratings.cc
LIB_RATING_SRC_HDR = SBB_ratings.h
#
LIB_YC_SRC = YieldCurve.cc
LIB_YC_SRC_HDR = YieldCurve.h
#
LIB_BOND_SRC = bond.cc
LIB_BOND_SRC_HDR = bond.h
#
LIB_PORTFOLIO_SRC = portfolio.cc
LIB_PORTFOLIO_SRC_HDR = portfolio.h
#
LIB_VAR_SRC = vargenerator.cc
LIB_VAR_SRC_HDR = vargenerator.h
#
LIB_OBJ = $(LIB_DATE_SRC:.cc=.o) $(LIB_RATING_SRC:.cc=.o) $(LIB_YC_SRC:.cc=.o) $(LIB_BOND_SRC:.cc=.o) $(LIB_PORTFOLIO_SRC:.cc=.o) $(LIB_VAR_SRC:.cc=.o)
$(LIB_OBJ) :  $(LIB_RATING_SRC_HDR) $(LIB_YC_SRC_HDR) $(LIB_BOND_SRC_HDR) $(LIB_PORTFOLIO_SRC_HDR) $(LIB_VAR_SRC_HDR)
LIB_TARGET = lib.a
#
SOCKET_HDR = sbb_socket.h
#
#
# the file where main() is
#MAIN_SRC = mmain.cc
MAIN_SRC = sbb_server.cc
MAINOBJ = $(MAIN_SRC:.cc=.o) 
$(MAINOBJ) : $(LIB_TARGET) $(LIB_UTIL_TARGET)
MAINTARGET=testdriver
ARCHIVE_OPTIONS = rucsv
$(LIB_UTIL_TARGET) : $(LIB_UTIL_OBJ)
	ar $(ARCHIVE_OPTIONS) $(LIB_UTIL_TARGET) $(LIB_UTIL_OBJ)
$(LIB_TARGET) : $(LIB_OBJ)
	ar $(ARCHIVE_OPTIONS) $(LIB_TARGET) $(LIB_OBJ)
$(MAINTARGET) : $(MAINOBJ) $(LIB_TARGET) $(LIB_UTIL_TARGET) $(SOCKET_HDR)
	$(CC) $(LDFLAGS) $(MAINOBJ) $(LIB_UTIL_TARGET) $(LIB_TARGET) -lnsl -lsocket -lresolv -o $(MAINTARGET)
#-lnsl -lsocket -lresolv
all : util lib test
test: $(MAINTARGET)
util: $(LIB_UTIL_TARGET)
lib : $(LIB_TARGET)
clean:
	rm -f *.o 
clobber:
	-rm -f *.o *.a $(MAINTARGET)
backup: clobber
	tar cvf ./LuyiZhang.tar *.*


