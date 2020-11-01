#include <bangtal>
#include <iostream>

using namespace bangtal;
using namespace std;


ObjectPtr board[8][8];
ScenePtr scene;
ObjectPtr blackPoint1;
ObjectPtr blackPoint2;
ObjectPtr whitePoint1;
ObjectPtr whitePoint2;
int getBlack;
int getWhite;

enum class State {
	BLANK,
	POSSIBLE,
	BLACK,
	WHITE
};
State board_state[8][8];
 
enum class Turn {
	BLACK,
	WHITE
};
Turn turn = Turn::BLACK; // 초기값은 BLACK으로 설정

void setState(int x, int y, State state) {   //현재 놓여지는 돌의 상태를 조정하는 함수      
	switch (state) {
	case State::BLANK: board[y][x]->setImage("Images/blank.png"); break;
	case State::POSSIBLE: board[y][x]->setImage(turn == Turn::BLACK ? "Images/black possible.png" : "Images/white possible.png"); break;
	case State::BLACK: board[y][x]->setImage("Images/black.png"); break;
	case State::WHITE: board[y][x]->setImage("Images/white.png"); break;
	}
	board_state[y][x] = state;
}

void checkPoint() {
	getBlack = 0;
	getWhite = 0;

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (board_state[y][x] == State::BLACK) getBlack++;
			else if (board_state[y][x] == State::WHITE) getWhite++;
		}
	}
	int m = getBlack % 10;
	int n = getWhite % 10;
	int i = getBlack / 10;
	int j = getWhite / 10;
	string blackP1 = "Images/L" + to_string(i) + ".png";
	string whiteP1 = "Images/L" + to_string(j) + ".png";
	string blackP2 = "Images/L" + to_string(m) + ".png";
	string whiteP2 = "Images/L" + to_string(n) + ".png";
	blackPoint1->setImage(blackP1);
	whitePoint1->setImage(whiteP1);
	blackPoint2->setImage(blackP2);
	whitePoint2->setImage(whiteP2);
}


bool checkPossible(int x, int y, int dx, int dy) {
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;

	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {    //한 라인을 계속 나아가며 체크함
		if (board_state[y][x] == other) {
			possible = true;
		}
		else if (board_state[y][x] == self) {
			return possible;
		}
		else {
			return false;
		}
	}
	return false;
}

bool checkPossible(int x, int y) {
	if (board_state[y][x] == State::BLACK) return false;
	if (board_state[y][x] == State::WHITE) return false;
	setState(x, y, State::BLANK);

	int delta[8][2] = {
		{0, 1},
		{1, 1},
		{1, 0},
		{1, -1},
		{0, -1},
		{-1, -1},
		{-1, 0},
		{-1, 1},
	};

	bool possible = false;

	for (auto d : delta) {		        // range for 지원 
		if (checkPossible(x, y, d[0], d[1])) possible = true;   //8방향 중 하나만 가능해도 possible이됨.
	}

	return possible;
}

void reverse(int x, int y, int dx, int dy) {
	State self = turn == Turn::BLACK ? State::BLACK : State::WHITE;
	State other = turn == Turn::BLACK ? State::WHITE : State::BLACK;
    //"조건 ? A : B"  >>  조건이 만족되면 A, 그렇지 않으면 B.

	bool possible = false;
	for (x += dx, y += dy; x >= 0 && x < 8 && y >= 0 && y < 8; x += dx, y += dy) {    //한 라인을 계속 나아가며 체크함
		if (board_state[y][x] == other) {
			possible = true;
		}
		else if (board_state[y][x] == self) {
			if (possible) {
				for (x -= dx, y -= dy; x >= 0 && x < 8 && y >= 0 && y < 8; x -= dx, y -= dy) {
					if (board_state[y][x] == other) {
						setState(x, y, self);
					}
					else {
						return;
					}
				}
			}
		}
		else {
			return;
		}
	}
	return;
}

void reverse(int x, int y) {
	int delta[8][2] = {
		{0, 1},
		{1, 1},
		{1, 0},
		{1, -1},
		{0, -1},
		{-1, -1},
		{-1, 0},
		{-1, 1},
	};

	bool possible = false;

	for (auto d : delta) {		        // range for 지원 
		reverse(x, y, d[0], d[1]);
	}
}

bool setPossible() {
	//8x8의 모든 위치에 대해 
	//놓여질 수 있으면 상태를 possible로 바꾼다
	bool possible = false;
	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			if (checkPossible(x, y)) {
				setState(x, y, State::POSSIBLE);
				possible = true;
			}
		}
	}
	return possible;
}

int main()
{
	setGameOption(GameOption::GAME_OPTION_INVENTORY_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_MESSAGE_BOX_BUTTON, false);
	setGameOption(GameOption::GAME_OPTION_ROOM_TITLE, false);

	scene = Scene::create("Othello", "Images/background.png");
	auto turnCheck = Object::create("Images/black turn.png", scene, 750, 380);
	blackPoint1 = Object::create("Images/L0.png", scene, 750, 220);
	blackPoint2 = Object::create("Images/L2.png", scene, 830, 220);
	whitePoint1 = Object::create("Images/L0.png", scene, 1070, 220);
	whitePoint2 = Object::create("Images/L2.png", scene, 1150, 220);

	for (int y = 0; y < 8; y++) {
		for (int x = 0; x < 8; x++) {
			board[y][x] = Object::create("Images/blank.png", scene, 40 + x * 80, 40 + y * 80);
			board[y][x]->setOnMouseCallback([&, x, y](ObjectPtr object, int, int, MouseAction action)-> bool {
				if (board_state[y][x] == State::POSSIBLE) {
					if (turn == Turn::BLACK) {
						setState(x, y, State::BLACK);
						reverse(x, y);
						turn = Turn::WHITE;
						turnCheck->setImage("Images/white turn.png");	
					}					
					else {
						setState(x, y, State::WHITE);
						reverse(x, y);
						turn = Turn::BLACK;
						turnCheck->setImage("Images/black turn.png");
					}
					if (!setPossible()) {
						turn = turn == Turn::BLACK ? Turn::WHITE : Turn::BLACK;   // toggle
						if (!setPossible()) {   // 둘다 안될 때
							if (getBlack > getWhite) showMessage("End Game! Black Win!");
							else if (getBlack < getWhite) showMessage("End Game! White WIn!");
							else if (getBlack == getWhite) showMessage("End Game! Draw!");
						}
					}
				}
				checkPoint();
				return true;
				});
			board_state[y][x] = State::BLANK;
		}
	}
	setState(3, 3, State::BLACK);
	setState(4, 4, State::BLACK);
	setState(3, 4, State::WHITE);
	setState(4, 3, State::WHITE);

	setPossible();

	startGame(scene);
}