NAME			=	Gomoku
BUILD_DIR	=	build

CXX				=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++17 -O3 -march=native -mtune=native \
						-flto -funroll-loops -finline-functions -ffast-math \
						-static-libstdc++ -fno-pie
CPPFLAGS	=	-I./include $(shell fltk-config --cxxflags)
LDFLAGS		=	-O3 -flto -no-pie -Wl,-z,now -Wl,--no-as-needed -static-libstdc++ -ldl $(shell fltk-config --ldflags)

SRCS			=	src/main.cpp \
						src/Board.cpp \
						src/BoardDraw.cpp \
						src/Timer.cpp \
						src/WinRules.cpp \
						src/Minimax.cpp \
						src/Utils.cpp \
						src/BitmapFont.cpp

HEADERS		=	include/Board.hpp \
						include/BitmapFont.hpp \
						include/Config.hpp \
						include/Gomoku.hpp \
						include/IBM_VGA_8x16.h \
						include/Minimax.hpp \
						include/Timer.hpp \
						include/Types.hpp \
						include/Utils.hpp \
						include/WinRules.hpp

OBJS			=	$(addprefix $(BUILD_DIR)/,$(SRCS:.cpp=.o))

all: $(NAME)

$(NAME): $(OBJS)
			$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

$(BUILD_DIR)/%.o:	%.cpp $(HEADERS)
			mkdir -p $(dir $@)
			$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

-include $(OBJS:.o=.d)

clean:
			rm -rf $(BUILD_DIR)

fclean: clean
			rm -f $(NAME) test

re: fclean all

valgrind-build:
			@$(MAKE) fclean
			@$(MAKE) CXXFLAGS="-Wall -Wextra -Werror -std=c++17 -O2 -g -static-libstdc++ -fno-pie" \
			LDFLAGS="-O2 -no-pie -Wl,-z,now -Wl,--no-as-needed -static-libstdc++ -ldl $$(fltk-config --ldflags)" \
			$(NAME)

valgrind:	valgrind-build
			valgrind --leak-check=full --show-leak-kinds=all \
			--track-origins=yes --suppressions=valgrind.supp ./$(NAME)

test: src/test.cpp
			$(CXX) -Wall -Wextra -Werror -std=c++17 -O2 -I./include \
			src/test.cpp src/WinRules.cpp src/Minimax.cpp src/Utils.cpp \
			-o test
			valgrind ./test

.PHONY: all clean fclean re valgrind valgrind-build test
