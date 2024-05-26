CXX = g++
CXXFLAGS = -std=c++11 -Wall
LIBS = -l gpiod

SRCDIR = src
BUILDDIR = build
TARGET = $(BUILDDIR)/main

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCDIR)/main.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)

run: $(TARGET)
	$(TARGET)