#
# makefile for the lexmark 2030 printer driver
#

# the main target: pbm2l2030 binary
pbm2l2030: pbm2l2030.o pbm.o
	gcc -o pbm2l2030 pbm2l2030.o pbm.o -O

# the pbm module
pbm.o: pbm.h pbm.c
	gcc -c pbm.c

# the main module
pbm2l2030.o: pbm.h pbm2l2030.c
	gcc -c pbm2l2030.c

# use this to test your printing configuration
test:
	a2ps -1 -p makefile

# install the driver into existing apsfilter (as root!)
install: pbm2l2030
	cp pbm2l2030 /usr/bin/pbm2ppa

# remove all files (to rebuild package)
clean:
	rm *.o pbm2l2030



