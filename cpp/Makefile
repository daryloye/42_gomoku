NAME			=	Gomoku

CXX				=	c++
CXXFLAGS		=	-Wall -Wextra -Werror -std=c++17 -O2 -static-libstdc++ -fno-pie
CPPFLAGS		=	-I./ $(shell fltk-config --cxxflags)
LDFLAGS			=	-no-pie -Wl,-z,now -Wl,--no-as-needed -static-libstdc++ -ldl $(shell fltk-config --ldflags)

SRCS			=	main.cpp

OBJS			=	$(SRCS:.cpp=.o)

all:			$(NAME)

$(NAME):		$(OBJS)
				$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

%.o:			%.cpp
				$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
				rm -f $(OBJS)

fclean:			clean
				rm -f $(NAME)

re:				fclean all

valgrind:		$(NAME)
				valgrind --leak-check=full --show-leak-kinds=all \
					--track-origins=yes \
					--suppressions=valgrind.supp ./$(NAME)

.PHONY:			all clean fclean re valgrind
