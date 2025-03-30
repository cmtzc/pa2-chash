CHASH = chash
CC = gcc
CFLAGS = -g -std=c17 -Wall
MV = mv
RM = rm -f
SUBMISSIONZIPFILE = PA2-chash.zip
ZIP = zip -9

CHASH_OBJECTS = chash.o crud.o utilities.o

.PRECIOUS: $(CHASH)

$(CHASH): $(CHASH_OBJECTS)
	$(CC) $(CFLAGS) -o $(CHASH) $(CHASH_OBJECTS) -mconsole

# rule for compiling individual .c files
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean: 
	$(RM) *~ *.o '#'*
	$(RM) $(CHASH).exe $(CHASH)
	$(RM) *.stackdump core
	$(RM) $(SUBMISSIONZIPFILE)

$(SUBMISSIONZIPFILE): *.c *.h \ Makefile

