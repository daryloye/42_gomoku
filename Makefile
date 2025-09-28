NAME = Gomoku
VENV_DIR = .venv
PIP = $(VENV_DIR)/bin/pip
PYTHON = $(VENV_DIR)/bin/python
PYTEST = $(VENV_DIR)/bin/pytest
REQUIREMENTS = requirements.txt

all: $(NAME)

$(NAME): setup
	echo "#!/bin/bash" > $(NAME)
	echo "$(PYTHON) main.py" >> $(NAME)
	chmod +x $(NAME)

setup:
	python3 -m venv $(VENV_DIR)
	$(PIP) install --upgrade pip setuptools wheel
	$(PIP) install -r $(REQUIREMENTS)

run:
	$(PYTHON) main.py

test:
	$(PYTEST)

clean:
	find . -type d -name __pycache__ -exec rm -rf {} +
	find . -type f -name "*.pyc" -delete
	find . -type f -name "*.pyo" -delete
	find . -type d -name "*.egg-info" -exec rm -rf {} +
	find . -type d -name ".pytest_cache" -exec rm -rf {} +

fclean: clean
	rm -f $(NAME)
	rm -rf $(VENV_DIR)

re: fclean all

.PHONY: all clean fclean re setup
