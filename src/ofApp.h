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
#define WALL_LEFT 0b0001 // 왼쪽 벽의 존재를 나타내는 Bit Flag
#define WALL_ALL 0b1111 // 모든 벽의 존재를 나타내는 Bit Flag

// route bit flag
#define ROUTE_VISITED 0b01 // 방문 여부를 나타내는 Bit Flag
#define ROUTE_ESCAPE 0b10 // 탈출 경로 여부를 나타내는 Bit Flag

// maze size offset
#define MAZE_OFFSET 10 // 미로 출력 여백

// maze size
#define MAZE_DEFAULT_HEIGHT 10 // 기본 미로 높이
#define MAZE_DEFAULT_WIDTH 10 // 기본 미로 너비

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

		int getRandBool(); // 랜덤 참/거짓값을 가져옴
		int getRandNum(int min, int max); // 주어진 범위 내의 랜덤 숫자를 가져옴
		
		void loadGame(bool reset = true); // 새로운 게임을 시작함. reset=true일 경우 플레이어 위치 및 경과 시간등의 값은 보존
		void createMaze(int h, int w, int* maze, pair<int, int> start); // 미로 생성 함수
		bool DFS(); // DFS 탐색 및 탈출 경로 계산 함수
		void setGoal(); // 목표 지점(도착지) 설정 함수
		void gameFinishCheck(); // 게임이 종료되었는지 체크하는 함수

		void drawRoute(); // 탈출 경로를 그리는 함수

		void freeMemory(); // 게임 종료 시 메모리 해제 함수
		

		int MAZE_HEIGHT, MAZE_WIDTH; // 미로의 크기

		int* maze; //미로 Graph
		int* route; // 경로 Array
		int maze_size, x_offset, y_offset; // 미로 출력 관련 값
		int initX, initY; // 초기 플레이어 시작 위치
		int playerX, playerY; // 현재 플레이어 위치
		int goalX, goalY; // 도착점 위치
		int lightRadius = 0; // focus light 반지름
		int lightIncrement = 0; // 사용자가 조절한 focus light 반지름
		int blockFound = 0; // 찾은 block의 수
		int move = 0; // 움직인 거리
		long long elapsedTime; // 게임 시작 후 경과 시간

		bool isPlaying; // 게임 중인지 체크하는 flag
		bool showHint; // hint를 보여주는 중인지 체크하는 flag
		bool isLight; // 미로 전체가 보이게 하는 중인지 체크하는 flag
		bool isWon; // 사용자가 승리했는지 체크하는 flag
		bool isGiveup; // 사용자가 패배(포기)했는지 체크하는 flag
		bool infiniteMode; // 무한 모드 여부 flag

		chrono::steady_clock::time_point game_clock; // 경과 시간 측정을 위한 clock

		ofFbo fbo; // focus light를 보여주기 위한 fbo

		ofColor escapeRouteColor; // 탈출 경로 표시 색상


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
