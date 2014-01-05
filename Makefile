CXX		?= g++
CXXFLAGS	:= -Wall -Wextra -std=c++11
LDFLAGS		:=

TARGET		:= gbemu
SOURCES		:= src/cpu/instr.cpp		\
		   src/memory/cartridge.cpp	\
		   src/memory/ram.cpp		\
		   src/utils/mapped_file.cpp	\
		   src/gbemu.cpp

OBJECTS		:= $(SOURCES:.cpp=.o)
DEPENDS		:= $(SOURCES:.cpp=.d)

VPATH		 = $(dir $(lastword $(MAKEFILE_LIST)))
CXXFLAGS	+= -I $(dir $(lastword $(MAKEFILE_LIST)))src

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -f $(OBJECTS)
	rm -f $(DEPENDS)

distclean: clean
	rm -f $(TARGET)

.PHONY: clean distclean

-include $(DEPENDS)
