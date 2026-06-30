# Makefile for Chess Engine (CLI) + SFML Frontend
#
# Just run `make` to build everything:
#   - frontend : SFML GUI with AI integration
#   - main     : standalone command-line engine

# Compilers
CC  = gcc
CXX = g++

# Optimization level (-O2 makes the engine search faster than the -O0 default)
OPT = -O2

# Compiler / linker flags
CFLAGS   = -Wall $(OPT) -c
CXXFLAGS = -Wall $(OPT)
LDFLAGS  = -lsfml-graphics -lsfml-window -lsfml-system -lm

# Build everything by default
all: frontend main
	@echo "Built: ./frontend (GUI) and ./main (CLI engine)"

# ---- Frontend (GUI) ----
frontend: frontend.o engine_interface.o main.o
	$(CXX) frontend.o engine_interface.o main.o -o frontend $(LDFLAGS)

frontend.o: frontend.cpp engine_interface.h
	$(CXX) $(CXXFLAGS) -c frontend.cpp

engine_interface.o: engine_interface.c engine_interface.h globals.h moves.h
	$(CC) $(CFLAGS) engine_interface.c

# Engine compiled without its own main() so the GUI can provide one
main.o: main.c moves.h binary_printer.h
	$(CC) $(CFLAGS) -DNO_MAIN main.c

# ---- Backend (standalone CLI engine) ----
main: main.c binary_printer.h moves.h globals.h
	$(CC) -Wall $(OPT) main.c -o main

# Run the GUI
run: frontend
	./frontend

clean:
	rm -f frontend.o engine_interface.o main.o frontend main

.PHONY: all run clean
