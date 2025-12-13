NAME			=	Gomoku
BUILD_DIR		=	build

CXX				=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++17 -O3 -march=native -mtune=native \
				-flto -funroll-loops -finline-functions -ffast-math \
				-static-libstdc++ -fno-pie
CPPFLAGS	=	-I./ $(shell fltk-config --cxxflags)
LDFLAGS		=	-O3 -flto -no-pie -Wl,-z,now -Wl,--no-as-needed -static-libstdc++ -ldl $(shell fltk-config --ldflags)

SRCS			=	main.cpp \
					Board.cpp \
					Timer.cpp \
					winRules.cpp \
					minimax2.cpp \
					utils.cpp \
					BitmapFont.cpp

HDRS			=	Gomoku.hpp BitmapFont.hpp

OBJS			=	$(addprefix $(BUILD_DIR)/,$(SRCS:.cpp=.o))

all:			$(NAME)

$(NAME):	$(OBJS)
				$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

$(BUILD_DIR)/%.o:	%.cpp $(HDRS)
				mkdir -p $(BUILD_DIR)
				$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
				rm -rf $(BUILD_DIR)

fclean:			clean
				rm -f $(NAME)

re:				fclean all

valgrind-build:
				@$(MAKE) fclean
				@$(MAKE) CXXFLAGS="-Wall -Wextra -Werror -std=c++17 -O2 -g -static-libstdc++ -fno-pie" \
						 LDFLAGS="-O2 -no-pie -Wl,-z,now -Wl,--no-as-needed -static-libstdc++ -ldl $$(fltk-config --ldflags)" \
						 $(NAME)

valgrind:	valgrind-build
				valgrind --leak-check=full --show-leak-kinds=all \
				--track-origins=yes --suppressions=valgrind.supp ./$(NAME)

.PHONY:		all clean fclean re valgrind valgrind-build
