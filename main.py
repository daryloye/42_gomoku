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

    try:
        pygame.init()
        pygame.display.set_caption("Gomoku")
    except Exception as e:
        print(f"Failed to initialize pygame: {e}")
        return

    while True:
        try:
            game = Game()
            result = game.run()
            if result == "quit":
                break
        except KeyboardInterrupt:
            print("\nGame interrupted. Exiting...")
            break
        except EOFError:
            print("\nReceived EOF (Ctrl+D). Exiting...")
            break
        except MemoryError:
            print("\nOut of memory! Try a smaller board size or close other applications.")
            print("Returning to menu...")
            continue
        except pygame.error as e:
            print(f"\nPygame error occurred: {e}")
            print("Attempting to continue...")
            continue
        except Exception as e:
            print(f"\nUnexpected error: {e}")
            print("Returning to menu...")
            continue

    try:
        pygame.quit()
    except Exception:
        pass


if __name__ == "__main__":
    main()
