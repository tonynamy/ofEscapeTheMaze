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
#define WALL_UP 0b1000
#define WALL_RIGHT 0b0100
#define WALL_DOWN 0b0010
#define WALL_LEFT 0b0001
#define WALL_ALL 0b1111

// route bit flag
#define ROUTE_DFS 0b001
#define ROUTE_BFS 0b010
#define ROUTE_ESCAPE 0b100
#define ROUTE_FULL 0b111

// maze size offset
#define MAZE_OFFSET 10

// maze size
#define MAZE_DEFAULT_HEIGHT 15
#define MAZE_DEFAULT_WIDTH 15

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
		int getRandBool(int seed, int i, int j, int w);
		int getRandNum(int min, int max);
		void createMaze(int i, int h, int w, int* maze, int* maze_set, int seed);
		void loadGame();
		void gameFinishCheck();

		int MAZE_HEIGHT, MAZE_WIDTH;

		int* maze; //�̷� Graph
		int* route; // ���
		int** visited;//�湮���θ� ������ ����
		int maze_col;//�̷�ĭ�� ���� �ε����� ����Ų��.
		int maze_row;//�̷�ĭ�� ���� �ε����� ����Ų��.
		int k;
		int isOpen; //������ ���ȴ����� �Ǵ��ϴ� ����. 0�̸� �ȿ��Ȱ� 1�̸� ���ȴ�.
		int isDFS;//DFS�Լ��� ������״��� �Ǵ��ϴ� ����. 0�̸� ������߰� 1�̸� �����ߴ�.
		int isBFS;//BFS�Լ��� ������״��� �Ǵ��ϴ� ����. 0�̸� ������߰� 1�̸� �����ߴ�.
		int maze_size, x_offset, y_offset;
		int initX, initY;
		int playerX, playerY;
		int goalX, goalY;
		int lightRadius = 0;
		int lightIncrement = 0;
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
        ofImage myImage;
		float windowWidth, windowHeight;
		HWND hWnd; // Application window
		HWND hWndForeground; // current foreground window

		// Example menu variables
		bool bShowInfo;
		bool bFullscreen;
		bool bTopmost;
		// Example functions
 		void doFullScreen(bool bFull);
		void doTopmost(bool bTop);

		ofColor bfsRouteColor;
		ofColor dfsRouteColor;
		ofColor escapeRouteColor;

		bool isPlaying;
		bool showHint;
		bool isLight;

		bool isWon;

};
