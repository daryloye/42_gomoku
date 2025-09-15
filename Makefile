NAME = Gomoku

all: $(NAME)

$(NAME):
	echo "#!/bin/bash" > $(NAME)
	echo "python3 main.py" >> $(NAME)
	chmod +x $(NAME)

clean:
	find . -type d -name __pycache__ -exec rm -rf {} +
	find . -type f -name "*.pyc" -delete
	find . -type f -name "*.pyo" -delete
	find . -type d -name "*.egg-info" -exec rm -rf {} +
	find . -type d -name ".pytest_cache" -exec rm -rf {} +

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
