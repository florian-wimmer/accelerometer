CXX = g++
CXXFLAGS = -std=c++11 -Wall -Iinc
LIBS = -l gpiod

SRCDIR = src
INCDIR = inc
BUILDDIR = build
TARGET = $(BUILDDIR)/main

SRC = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SRC))

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ) | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILDDIR)

run: $(TARGET)
	$(TARGET)