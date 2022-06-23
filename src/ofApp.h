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
#define WALL_UP 0b1000 // �� ���� ���縦 ��Ÿ���� Bit Flag
#define WALL_RIGHT 0b0100  // ������ ���� ���縦 ��Ÿ���� Bit Flag
#define WALL_DOWN 0b0010 // �Ʒ� ���� ���縦 ��Ÿ���� Bit Flag
#define WALL_LEFT 0b0001 // ���� ���� ���縦 ��Ÿ���� Bit Flag
#define WALL_ALL 0b1111 // ��� ���� ���縦 ��Ÿ���� Bit Flag

// route bit flag
#define ROUTE_VISITED 0b01 // �湮 ���θ� ��Ÿ���� Bit Flag
#define ROUTE_ESCAPE 0b10 // Ż�� ��� ���θ� ��Ÿ���� Bit Flag

// maze size offset
#define MAZE_OFFSET 10 // �̷� ��� ����

// maze size
#define MAZE_DEFAULT_HEIGHT 10 // �⺻ �̷� ����
#define MAZE_DEFAULT_WIDTH 10 // �⺻ �̷� �ʺ�

class ofApp : public ofBaseApp {

	public:

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		int getRandBool(); // ���� ��/�������� ������
		int getRandNum(int min, int max); // �־��� ���� ���� ���� ���ڸ� ������
		
		void loadGame(bool reset = true); // ���ο� ������ ������. reset=true�� ��� �÷��̾� ��ġ �� ��� �ð����� ���� ����
		void createMaze(int h, int w, int* maze, pair<int, int> start); // �̷� ���� �Լ�
		bool DFS(); // DFS Ž�� �� Ż�� ��� ��� �Լ�
		void setGoal(); // ��ǥ ����(������) ���� �Լ�
		void gameFinishCheck(); // ������ ����Ǿ����� üũ�ϴ� �Լ�

		void drawRoute(); // Ż�� ��θ� �׸��� �Լ�

		void freeMemory(); // ���� ���� �� �޸� ���� �Լ�
		

		int MAZE_HEIGHT, MAZE_WIDTH; // �̷��� ũ��

		int* maze; //�̷� Graph
		int* route; // ��� Array
		int maze_size, x_offset, y_offset; // �̷� ��� ���� ��
		int initX, initY; // �ʱ� �÷��̾� ���� ��ġ
		int playerX, playerY; // ���� �÷��̾� ��ġ
		int goalX, goalY; // ������ ��ġ
		int lightRadius = 0; // focus light ������
		int lightIncrement = 0; // ����ڰ� ������ focus light ������
		int blockFound = 0; // ã�� block�� ��
		int move = 0; // ������ �Ÿ�
		long long elapsedTime; // ���� ���� �� ��� �ð�

		bool isPlaying; // ���� ������ üũ�ϴ� flag
		bool showHint; // hint�� �����ִ� ������ üũ�ϴ� flag
		bool isLight; // �̷� ��ü�� ���̰� �ϴ� ������ üũ�ϴ� flag
		bool isWon; // ����ڰ� �¸��ߴ��� üũ�ϴ� flag
		bool isGiveup; // ����ڰ� �й�(����)�ߴ��� üũ�ϴ� flag
		bool infiniteMode; // ���� ��� ���� flag

		chrono::steady_clock::time_point game_clock; // ��� �ð� ������ ���� clock

		ofFbo fbo; // focus light�� �����ֱ� ���� fbo

		ofColor escapeRouteColor; // Ż�� ��� ǥ�� ����


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


};
