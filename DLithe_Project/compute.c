#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

struct user {
	char name[20];
	char lastName[20];
};

void startEngine(int highScore, struct user firstUser);
void endGame(int score, int highScore, int diY, int diX, struct user firstUser);
void showDinasour(int diY, int diX);
void startMenu();
int computeTime(int delayTime);
int checkGame(int y, int x, int diY, int diX);

// Check if the game is going to be finished in this turn of loop
int checkGame(int y, int x, int diY, int diX) {
	if (diY == y) {
		if (abs((diX+14)-x) <= 4) {
			return 0;
		}
	}
	return 1;
}

// Make game faster
int computeTime(int delayTime) {
	if (delayTime >= 250000) {
		delayTime -= 900;
	}
	else if (delayTime >= 200000) {
		delayTime -= 600;
	}
	else {
		delayTime -= 200;
	}
	return delayTime;
}

// The very first menu
void startMenu() {
	struct user firstUser;
	int highScore;
    // Read high score from file
	FILE *highScoreFile;
	highScoreFile = fopen("./highScore.txt", "r");
	fscanf(highScoreFile, "%d", &highScore);
	fclose(highScoreFile);
	int maxX = getmaxx(stdscr)/2;
	int maxY = getmaxy(stdscr)/2;
	mvprintw(maxY+2, maxX-28, "Input details and press Enter to start Game.");
	mvprintw(maxY, maxX-21, "Press space bar key to jump !");
	mvprintw(maxY+4, maxX-28, "Name: ");
	getstr(firstUser.name);
	mvprintw(maxY+5, maxX-28, "Last name: ");
	getstr(firstUser.lastName);
	noecho();
	startEngine(highScore, firstUser);
}

// Which dinosaur should be printed
void showDinasour(int diY, int diX) {
	static int counter = 0;
	if (counter == 0) {
		dinasour1(diY, diX);
		counter++;
	}
	else {
		dinasour2(diY, diX);
		counter--;
	}
}

// Finishing function
void endGame(int score, int highScore, int diY, int diX, struct user firstUser) {
	nodelay(stdscr, FALSE);
	init_pair(2,COLOR_RED,COLOR_BLACK);
    // Save high score
	if (score > highScore) {
		highScore = score;
		FILE *highScoreFile;
		highScoreFile = fopen("./highScore.txt", "w");
		fprintf(highScoreFile, "%d", highScore);
		fclose(highScoreFile);
		highScoreFile = fopen("./highScoreName.txt", "w");
		fprintf(highScoreFile, "First Name : %s\n", firstUser.name);
		fprintf(highScoreFile, "Last Name : %s\n", firstUser.lastName);
		fclose(highScoreFile);
	}
	int maxX = getmaxx(stdscr)/2;
	int maxY = getmaxy(stdscr)/2;
	attron(COLOR_PAIR(2));
	showLoss(maxY, maxX);
	mvprintw(diY-4, diX, "          X-X ");
	mvprintw(diY, diX, "      ||");
	char keyToExit = getch();
    // Exit or reset game
	if (keyToExit == 'r') {
		attroff(COLOR_PAIR(2));
		startEngine(highScore, firstUser);
	}
	else if (keyToExit == 'q') {
		return;
	}
	else {
		endGame(score, highScore, diY, diX, firstUser);
	}
}

// The main engine!
void startEngine(int highScore, struct user firstUser) {
    srand(time(NULL));
	struct user highScorer;
	int x, y, diX=5,score=0, delayTime = 300000, gameStatus=1, cactusNum=0;
	int maxX=getmaxx(stdscr);
	x = maxX-20;
	y = getmaxy(stdscr)-6;
	int diY = y;
	char userInput;
	int jumping=-1;
    clear();
	FILE *fptr = fopen("highScoreName.txt","r");
	while(fscanf(fptr,"First Name : %s\n Last Name : %s\n",highScorer.name,highScorer.lastName)==2);
	fclose(fptr);
	nodelay(stdscr, TRUE);
  	init_pair(1,COLOR_WHITE,COLOR_BLACK);
	init_pair(4,COLOR_BLUE,COLOR_BLACK);
	while (gameStatus == 1) {
		userInput = getch();
		// Show day or night
		if (score % 40 >= 20 ) {
            clear();
			attron(COLOR_PAIR(4));
			moon(10, (maxX/2)-10);			
		}
		else {
			attron(COLOR_PAIR(1));
			sun(10, (maxX/2)-10);			
		}
		score++;
        // Show informations
		mvprintw(1, 6, "First Name : %s", firstUser.name);
		mvprintw(2, 6, "Last Name : %s", firstUser.lastName);
		mvprintw(1, getmaxx(stdscr)-45, "Current Highscore Holder : %s %s", highScorer.name,highScorer.lastName);
		mvprintw(2, getmaxx(stdscr)-45, "Current Highscore : %d", highScore);
		mvprintw(3, getmaxx(stdscr)-45, "Current Score : %d", score);
		// ----------
		box(stdscr, ACS_VLINE, ACS_HLINE);
		//for clearing screen
		clearDinasourUp(diY, diX);
		if (x <= 7) {
			x = getmaxx(stdscr)-20;
            cactusNum = rand() % 2;
		}
        // if input is equal to ' ' then jump
		if (userInput == ' ' && jumping<0) {
			diY -= 7;
			jumping = 3;
		}
		showDinasour(diY, diX);
		if (userInput == ' ') {
			clearDinasourDown(diY, diX);		
		}
		if (x-diX <= 7) {
			x -= 10;
		}
        if (cactusNum == 0) {
        	cactus1(y, x);
		}
		else {
			cactus2(y, x);
		}
		if (x-diX <= 7) {
			x += 10;
		}
		gameStatus = checkGame(y, x, diY, diX);
        // Bring back dinosaur
		jumping--;
		if (jumping==0) {
			diY += 7;
		}
		mvhline(y+1, 1, '-', getmaxx(stdscr)-3);
        refresh();
        clearCactus1(y, x);
		refresh();
        usleep(delayTime);
        x -= 7;
        delayTime = computeTime(delayTime);
		userInput = 'q';
	}
    // When code reaches here, means that user is loss
	endGame(score, highScore, diY, diX, firstUser);
	attroff(COLOR_PAIR(1));
}