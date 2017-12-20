exe := $(shell pwd | sed "s/.*\///").exe
srcs := main.cpp stream.cpp request.cpp server.cpp response.cpp \
		connection.cpp common.cpp
objs := $(patsubst %.cpp,%.o,$(srcs))

CFLAGS += -g -Wall -Werror

default: $(exe)

DEPS := $(objs:.o=.d)

-include $(DEPS)

%.o: %.cpp
	@$(CXX) $(CFLAGS) -MM -MT $@ -MF $(patsubst %.o,%.d,$@) $<
	$(CXX) $(CFLAGS) -c -o $@ $<

$(exe) : $(objs)
	$(CXX) $(CFLAGS) -o $@ $(objs)
	chmod +x $(exe)

clean: 
	-@rm *.d *.o $(exe)
