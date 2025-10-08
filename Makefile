NAME = Gomoku
VENV_DIR = .venv
PIP = $(VENV_DIR)/bin/pip
PYTHON = $(VENV_DIR)/bin/python
PYTEST = $(VENV_DIR)/bin/pytest
REQUIREMENTS = requirements.txt
VENV_STAMP = $(VENV_DIR)/.stamp

SRCS = main.py \
       core/game.py \
       core/rules.py \
       core/move.py \
       core/utils.py \
       core/minmax.py \
       player/player.py \
       player/ai.py \
       ui/screen.py \
       ui/menu.py \
       ui/stones.py \
       config.py

all: $(NAME)

$(VENV_STAMP): $(REQUIREMENTS)
	if [ ! -d "$(VENV_DIR)" ]; then \
		python3 -m venv $(VENV_DIR); \
	fi
	$(PIP) install --upgrade pip setuptools wheel > /dev/null 2>&1
	$(PIP) install -r $(REQUIREMENTS) > /dev/null 2>&1
	touch $(VENV_STAMP)

$(NAME): $(VENV_STAMP) $(SRCS)
	echo "#!/bin/bash" > $(NAME)
	echo "cd \$$(dirname \$$0)" >> $(NAME)
	echo "$(PYTHON) main.py" >> $(NAME)
	chmod +x $(NAME)

run: $(VENV_STAMP)
	$(PYTHON) main.py

test: $(VENV_STAMP)
	$(PYTEST)

clean:
	find . -type d -name __pycache__ -exec rm -rf {} + 2>/dev/null || true
	find . -type f -name "*.pyc" -delete 2>/dev/null || true
	find . -type f -name "*.pyo" -delete 2>/dev/null || true
	find . -type d -name "*.egg-info" -exec rm -rf {} + 2>/dev/null || true
	find . -type d -name ".pytest_cache" -exec rm -rf {} + 2>/dev/null || true

fclean: clean
	rm -f $(NAME)
	rm -rf $(VENV_DIR)

re: fclean all

.PHONY: all clean fclean re run test
