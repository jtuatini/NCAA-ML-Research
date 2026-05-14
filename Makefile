# NCAA UROP — Makefile for the C++ AP-rank classifier.

CXX        = g++
CXXFLAGS   = -std=c++17 -Wall -Wextra -pedantic
EXECUTABLE = classifier
SOURCES    = classifier.cpp

INPUT_2024 = team_season_summary_2024.csv
INPUT_2025 = team_season_summary_2025.csv

.PHONY: all release debug run run-2024 run-2025 clean

all: release

release: CXXFLAGS += -O3 -DNDEBUG
release: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CXX) $(CXXFLAGS) $< -o $@

run: run-2024 run-2025

run-2024: release
	./$(EXECUTABLE) < $(INPUT_2024)

run-2025: release
	./$(EXECUTABLE) < $(INPUT_2025)

clean:
	rm -f $(EXECUTABLE) $(EXECUTABLE).exe
	rm -rf $(EXECUTABLE).dSYM
