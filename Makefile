Cxx=g++
CXXFLAGS = -I/usr/local/include/opencv2 -O0 -g3 -Wall -c
LIBS =    -L/usr/lib -lpthread -ldl -lm -std=gnu++0x -std=c++0x -lopencv_core -lopencv_highgui -lopencv_imgproc

MAP2GRAPH_SOURCES = map2graph.cpp
MAP2GRAPH_OBJECTS = map2graph.o
MAP2GRAPH_EXECUTABLE = map2graph

.PHONY: all

all: $(MAP2GRAPH_OBJECTS)
	@echo 'Building executable:'
	$(CXX) -o $(MAP2GRAPH_EXECUTABLE) $(MAP2GRAPH_OBJECTS) $(LIBS)
	@echo 'Building Finished!'

clean:
	rm -rf $(MAP2GRAPH_EXECUTABLE) $(MAP2GRAPH_OBJECTS)
	@echo 'Clean Finished!'

%.d: %.cpp
	$(CXX) -MM -MF $@ $<

%.o: %.d

-include $(OBJS:.o=.d)
