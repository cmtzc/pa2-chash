CHASH = chash
CC = gcc
CFLAGS = -g -std=gnu99 -Wall
MV = mv
RM = rm -f
SUBMISSIONZIPFILE = PA2-chash.zip
OUTPUT = output.txt
ZIP = zip -9

CHASH_OBJECTS = utilities.o crud.o command.o chash.o rwlock.o

.PRECIOUS: $(CHASH)

$(CHASH): $(CHASH_OBJECTS)
	$(CC) $(CFLAGS) -o $(CHASH) $(CHASH_OBJECTS)

# rule for compiling individual .c files
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean: 
	$(RM) *~ *.o '#'*
	$(RM) $(CHASH).exe $(CHASH)
	$(RM) *.stackdump core
	$(RM) $(SUBMISSIONZIPFILE)
	$(RM) $(OUTPUT)

.PHONY: run
run: $(CHASH)
	./$(CHASH)

$(SUBMISSIONZIPFILE): *.c *.h \ Makefile

