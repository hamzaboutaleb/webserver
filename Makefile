SRCS = $(shell find ./src -name "*.cpp")
OBJS = $(patsubst ./src/%.cpp, build/%.o, $(SRCS))

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -I./include -std=c++98
TARGET = web-serv

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $@ $^

build/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -rf build

fclean: clean
	@rm -f $(TARGET)

re: fclean all

run: re
	@clear
	@echo "Running $(TARGET)... \n"
	@./$(TARGET)
.PHONY: all clean fclean re