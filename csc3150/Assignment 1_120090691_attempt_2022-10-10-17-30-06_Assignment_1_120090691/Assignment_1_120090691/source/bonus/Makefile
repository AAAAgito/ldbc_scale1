CFILES:= $(shell ls|grep .cpp)
PROGS:=$(patsubst %.cpp,%,$(CFILES))

all: $(PROGS)

%:%.cpp
	$(CXX) -std=c++11 -o $@ $<

clean:$(PROGS)
	rm $(PROGS)
