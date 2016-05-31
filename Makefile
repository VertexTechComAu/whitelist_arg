
#The main compiler
CC=g++

#Source folder
SRCDIR := src

#Build folder
BUILDDIR := build

#BIN folder
BINDIR := bin

#Target output
TARGET := $(BINDIR)/whitelist

#Source extension
SRCEXT := cpp

#All the cpp files to be compiled
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))



all:
	@mkdir -p $(BINDIR)
	$(CC) $(SOURCES) -o $(TARGET)

clean:
	rm $(TARGET)
