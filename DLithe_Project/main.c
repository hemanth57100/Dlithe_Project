#include <ncurses.h>
#include "appearance.c"
#include "compute.c"

int main() {
	// Initialize the curses library
	initscr();
	// Initialize color support
	start_color();
	// Hide the cursor
	curs_set(FALSE);
	 // Call a function to start the menu or user interface
	startMenu();
	// Clean up and close the curses library
	endwin();
}
