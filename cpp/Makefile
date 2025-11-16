NAME			=	Gomoku
BUILD_DIR		=	build

CXX				=	c++
CXXFLAGS	=	-Wall -Wextra -Werror -std=c++17 -O2 -static-libstdc++ -fno-pie
CPPFLAGS	=	-I./ $(shell fltk-config --cxxflags)
LDFLAGS		=	-no-pie -Wl,-z,now -Wl,--no-as-needed -static-libstdc++ -ldl $(shell fltk-config --ldflags)

SRCS			=	main.cpp \
					Board.cpp \
					Timer.cpp \
					winRules.cpp \
					minimax.cpp \
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

valgrind:	$(NAME)
				valgrind --leak-check=full --show-leak-kinds=all \
				--track-origins=yes --suppressions=valgrind.supp ./$(NAME)

.PHONY:		all clean fclean re valgrind
