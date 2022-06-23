/*

	ofxWinMenu basic example - ofApp.h

	Copyright (C) 2016-2017 Lynn Jarvis.

	http://www.spout.zeal.co

	=========================================================================
	This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
	=========================================================================
*/

	#pragma once

#include "ofMain.h"
#include "ofxWinMenu.h" // Menu addon'

#include <vector>
#include <random>
#include <thread>

// wall bit flag
#define WALL_UP 0b1000 // 위 벽의 존재를 나타내는 Bit Flag
#define WALL_RIGHT 0b0100  // 오른쪽 벽의 존재를 나타내는 Bit Flag
#define WALL_DOWN 0b0010 // 아래 벽의 존재를 나타내는 Bit Flag
#define WALL_LEFT 0b0001 // 윗 벽의 존재를 나타내는 Bit Flag
#define WALL_ALL 0b1111 // 모든 벽의 존재를 나타내는 Bit Flag

// route bit flag
#define ROUTE_VISITED 0b01
#define ROUTE_ESCAPE 0b10

// maze size offset
#define MAZE_OFFSET 10

// maze size
#define MAZE_DEFAULT_HEIGHT 10
#define MAZE_DEFAULT_WIDTH 10

// wall char
#define WALL_VERTICAL "|"
#define WALL_HORIZONTAL "-"
#define WALL_CORNER "+"
#define WALL_BLANK " "
#define MAZE_SPACE " "

class ofApp : public ofBaseApp {

	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key); // Traps escape key if exit disabled
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void freeMemory();
		bool DFS();
		void drawRoute();
		int getRandBool();
		int getRandNum(int min, int max);
		void createMaze(int h, int w, int* maze, pair<int, int> start);
		void loadGame(bool reset = true);
		void gameFinishCheck();

		int MAZE_HEIGHT, MAZE_WIDTH;

		int* maze; //미로 Graph
		int* route; // 경로
		int** visited;//방문여부를 저장할 포인
		int maze_col;//미로칸의 열의 인덱스를 가리킨다.
		int maze_row;//미로칸의 행의 인덱스를 가리킨다.
		int k;
		int isOpen; //파일이 열렸는지를 판단하는 변수. 0이면 안열렸고 1이면 열렸다.
		int isDFS;//DFS함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
		int isBFS;//BFS함수를 실행시켰는지 판단하는 변수. 0이면 실행안했고 1이면 실행했다.
		int maze_size, x_offset, y_offset;
		int initX, initY;
		int playerX, playerY;
		int goalX, goalY;
		int lightRadius = 0;
		int lightIncrement = 0;
		int blockFound = 0;
		long long elapsedTime;

		chrono::steady_clock::time_point game_clock;

		ofFbo fbo;

		void setGoal();

		// Menu
		ofxWinMenu * menu; // Menu object
		void appMenuFunction(string title, bool bChecked); // Menu return function

		// Used by example app
		ofTrueTypeFont myFont;
		ofTrueTypeFont messageFont;

		float windowWidth, windowHeight;
		HWND hWnd; // Application window
		HWND hWndForeground; // current foreground window

		bool bFullscreen;
 		void doFullScreen(bool bFull);

		ofColor escapeRouteColor;

		bool isPlaying;
		bool showHint;
		bool isLight;
		bool isWon;
		bool isGiveup;
		bool infiniteMode;

};
