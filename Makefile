CXX = g++
CXXFLAGS = -std=c++17
LDFLAGS = -l/usr/include/lua5.4 -llua5.4 -lpqxx -lpq -lcurl -lboost_system -pthread

SRCDIR = src
BUILDDIR = bin
TARGET = $(BUILDDIR)/lunar

SRCFILES = $(wildcard $(SRCDIR)/*.cpp)
OBJFILES = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SRCFILES))

all: $(TARGET)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

# Builds the target for ther CLI
$(TARGET): $(BUILDDIR) $(OBJFILES) $(CXX) $(CXXFLAGS) $(OBJFILES) $(LDFLAGS) -o $(TARGET)

# Compiling source files into object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp $(CXX) $(CXXFLAGS) -c $ < -o $@

clean:
	rm -rf $(BUILDDIR)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
