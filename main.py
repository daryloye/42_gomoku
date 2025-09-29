#!/usr/bin/env python3
import os
os.environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "1"
import warnings
warnings.filterwarnings("ignore", category=UserWarning, module="pygame.pkgdata")
from core.game import Game
import pygame
import sys
import signal


def signal_handler(signum, frame):
    """Handle keyboard interrupts gracefully"""
    if signum == signal.SIGINT:
        print("\nReceived Ctrl+C. Exiting gracefully...")
    elif signum == signal.SIGTERM:
        print("\nReceived termination signal. Exiting gracefully...")
    elif hasattr(signal, 'SIGQUIT') and signum == signal.SIGQUIT:
        print("\nReceived Ctrl+\\. Forcing immediate exit...")

    pygame.quit()
    sys.exit(0)


def main():
    signal.signal(signal.SIGINT, signal_handler) 
    signal.signal(signal.SIGTERM, signal_handler)

    if hasattr(signal, 'SIGQUIT'):
        signal.signal(signal.SIGQUIT, signal_handler)

    print("Running validation checks...")

    try:
        from tests.test_validate import validate_all_examples
        if not validate_all_examples():
            print("Validation failed. App will not start.")
            sys.exit(1)
        print("Validation passed. Starting game...")
    except Exception as e:
        print(f"Validation error: {e}")
        sys.exit(1)

    pygame.init()
    pygame.display.set_caption("Gomoku")

    while True:
        try:
            game = Game()
            result = game.run()
            if result == "quit":
                break
        except KeyboardInterrupt:
            # Handle Ctrl+C during game execution
            print("\nGame interrupted. Exiting...")
            break
        except EOFError:
            # Handle Ctrl+D (EOF)
            print("\nReceived EOF (Ctrl+D). Exiting...")
            break
        except Exception:
            break

    pygame.quit()


if __name__ == "__main__":
    main()
