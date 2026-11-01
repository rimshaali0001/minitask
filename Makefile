CXX = g++
CXXFLAGS = -Wall -std=c++17

all: test test_processes

test: system_info.cpp signal_control.cpp test_main.cpp
	$(CXX) $(CXXFLAGS) -o test system_info.cpp signal_control.cpp test_main.cpp

test_processes: process_reader.cpp test_processes.cpp
	$(CXX) $(CXXFLAGS) -o test_processes process_reader.cpp test_processes.cpp

clean:
	rm -f test test_processes
