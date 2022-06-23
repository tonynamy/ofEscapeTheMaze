/*

	ofxWinMenu basic example - ofApp.cpp

	Example of using ofxWinMenu addon to create a menu for a Microsoft Windows application.

	Copyright (C) 2016-2017 Lynn Jarvis.

	https://github.com/leadedge

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

	03.11.16 - minor comment cleanup
	21.02.17 - rebuild for OF 0.9.8

*/
#include "ofApp.h"
#include <iostream>
using namespace std;
//--------------------------------------------------------------
void ofApp::setup() {

	ofSetWindowTitle("ofEscapeTheMaze"); // Set the app name on the title bar
	ofSetFrameRate(15);
	ofBackground(255, 255, 255);
	
	// 각종 변수 초기화
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();

	MAZE_HEIGHT = MAZE_DEFAULT_HEIGHT;
	MAZE_WIDTH = MAZE_DEFAULT_WIDTH;

	// Centre on the screen
	ofSetWindowPosition((ofGetScreenWidth() - windowWidth) / 2, (ofGetScreenHeight() - windowHeight) / 2);

	// Load a font rather than the default
	myFont.loadFont("verdana.ttf", 15, true, true);
	messageFont.loadFont("verdana.ttf", 25, true, true);

	// Load an image for the example
	//myImage.loadImage("lighthouse.jpg");

	// Window handle used for topmost function
	hWnd = WindowFromDC(wglGetCurrentDC());

	// Disable escape key exit so we can exit fullscreen with Escape (see keyPressed)
	ofSetEscapeQuitsApp(false);

	lightRadius = maze_size;
	lightIncrement = 0;
	elapsedTime = 0;

	//
	// ofxWinMenu 추가
	//

	// A new menu object with a pointer to this class
	menu = new ofxWinMenu(this, hWnd);

	// Register an ofApp function that is called when a menu item is selected.
	// The function can be called anything but must exist. 
	// See the example "appMenuFunction".
	menu->CreateMenuFunction(&ofApp::appMenuFunction);

	// Create a window menu
	HMENU hMenu = menu->CreateWindowMenu();

	//
	// Create a "File" popup menu
	//
	HMENU hPopup = menu->AddPopupMenu(hMenu, "Game");

	//
	// Add popup items to the File menu
	//

	// Open an maze file
	isPlaying = false;
	showHint = false;
	isWon = false;
	isGiveup = false;
	infiniteMode = false;

	menu->AddPopupItem(hPopup, "New Game", false, false); // Not checked and not auto-checked
	menu->AddPopupItem(hPopup, "Giveup", false, false); // Not checked and not auto-checked

	menu->AddPopupSeparator(hPopup);
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check
	menu->AddPopupItem(hPopup, "Set Maze Size", false, false); // Not checked and not auto-check
	menu->AddPopupItem(hPopup, "Infinite Mode", false, false); // Not checked and not auto-check

	// Final File popup menu item is "Exit" - add a separator before it
	menu->AddPopupSeparator(hPopup);
	menu->AddPopupItem(hPopup, "Exit", false, false);

	//
	// Help popup menu
	//
	hPopup = menu->AddPopupMenu(hMenu, "Help");
	menu->AddPopupItem(hPopup, "About", false, false); // No auto check

	// Set the menu to the window
	menu->SetWindowMenu();

	escapeRouteColor = ofColor(129, 205, 251);

} // end Setup


//
// Menu function
//
// This function is called by ofxWinMenu when an item is selected.
// The the title and state can be checked for required action.
// 
void ofApp::appMenuFunction(string title, bool bChecked) {

	ofFileDialogResult result;
	string filePath;
	size_t pos;

	//
	// File menu
	//
	if (title == "New Game") {
		loadGame();
	}
	if (title == "Exit") {
		freeMemory();
		ofExit(); // Quit the application
	}

	if (title == "Set Maze Size") {
		// 미로 크기 설정
		string a = ofSystemTextBoxDialog("Size of Maze?", to_string(MAZE_HEIGHT));
		int size = std::stoi(a);

		isPlaying = false;
		isWon = false;
		isGiveup = false;

		// 메모리 해제
		freeMemory();

		MAZE_HEIGHT = size;
		MAZE_WIDTH = size;
	}

	//
	// Window menu
	//

	if (title == "Full screen") {
		// 전체화면 기능
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	if (title == "Infinite Mode") {
		// infinite mode 토글
		infiniteMode = !infiniteMode;
		menu->SetPopupItem("Infinite Mode", infiniteMode);
	}

	if (title == "Giveup") {
		// 플레이 중일때만 게임 포기 가능
		if (isPlaying) {
			isGiveup = true;
		}
	}

	//
	// Help menu
	//
	if (title == "About") {
		// 안내 메시지
		ofSystemAlertDialog("ofEscapeTheMaze\n2022-1 Comsil Final Project\nMade by Sogang Univ. tonynamy@apperz.co.kr");
	}

} // end appMenuFunction


//--------------------------------------------------------------
void ofApp::update() {

}


//--------------------------------------------------------------
void ofApp::draw() {

	char str[256];
	//ofBackground(0, 0, 0, 0);
	ofSetColor(100);
	ofSetLineWidth(5);

	// 플레이 중일 때
	if (isPlaying) {

		// 미로 벽을 그리는 부분
		maze_size = (min(ofGetHeight(), ofGetWidth()) - MAZE_OFFSET) / max(MAZE_WIDTH, MAZE_HEIGHT);
		lightRadius = maze_size;
		x_offset = (ofGetWidth() - maze_size * MAZE_WIDTH) / 2;
		y_offset = (ofGetHeight() - maze_size * MAZE_HEIGHT) / 2;


		// 각 미로칸에 대하여 벽(선)을 그림
		for (int i = 0; i < MAZE_HEIGHT; i++) {
			for (int j = 0; j < MAZE_WIDTH; j++) {

				if (maze[i * MAZE_WIDTH + j] & WALL_UP) ofDrawLine(x_offset+j * maze_size, y_offset+i * maze_size, x_offset+(j + 1) * maze_size, y_offset + i * maze_size);
				if (maze[i * MAZE_WIDTH + j] & WALL_DOWN) ofDrawLine(x_offset+j * maze_size, y_offset + (i + 1) * maze_size, x_offset+(j + 1) * maze_size, y_offset + (i + 1) * maze_size);
				if (maze[i * MAZE_WIDTH + j] & WALL_LEFT) ofDrawLine(x_offset+j * maze_size, y_offset + i * maze_size, x_offset+j * maze_size, y_offset + (i + 1) * maze_size);
				if (maze[i * MAZE_WIDTH + j] & WALL_RIGHT) ofDrawLine(x_offset+(j + 1) * maze_size, y_offset + i * maze_size, x_offset+(j + 1) * maze_size, y_offset + (i + 1) * maze_size);

			}
		}

		// hint가 true일 경우에는 DFS 탈출 경로를 그림
		if (showHint) {
			drawRoute();
		}

		// 플레이어의 위치 및 도착점을 그림
		int cx = playerX * maze_size + maze_size / 2 + x_offset;
		int cy = playerY * maze_size + maze_size / 2 + y_offset;

		int gx = goalX * maze_size + maze_size / 2 + x_offset;
		int gy = goalY * maze_size + maze_size / 2 + y_offset;

		int player_size = maze_size / 3;

		ofSetRectMode(OF_RECTMODE_CENTER);
		ofDrawRectangle(cx, cy, player_size, player_size);

		ofSetColor(ofColor::red);
		ofDrawRectangle(gx, gy, player_size, player_size);

		// focusLight가 켜져있거나 게임이 종료된 경우에는 focusLight 기능 작동
		if (isLight || isWon || isGiveup) {

			// alpha값
			fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

			// 새로운 레이어
			fbo.begin();
			ofClear(ofColor::black);

			int radiusStepSize = 3;
			int alpha = 3;
			int maxOffsetDistance = 100;

			auto a = ofColor(255, 255, 255, 255);
			auto b = ofColor(255, 255, 255, 100);

			// blendmode - subtract를 이용하여 mask 구성
			ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);

			for (int radius = lightRadius + lightIncrement; radius > 0; radius -= radiusStepSize) {

				// 가장자리로 갈 수록 옅어지는 효과 구현
				ofSetColor(a.lerp(b, (lightRadius + lightIncrement) / radius));

				ofDrawCircle(cx, cy, radius);

			}

			ofDisableBlendMode();

			fbo.end();

			ofSetRectMode(OF_RECTMODE_CORNER);

			// 레이어 적용
			fbo.draw(0.f, 0.f);
		}

	}

	// 플레이 중일때는 경과 시간 갱신
	if (isPlaying && !isWon && !isGiveup) {
		elapsedTime = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - game_clock).count();
	}

	// 경과 시간 표시
	if (isPlaying) {
		ofSetColor(100);
		sprintf(str, "Move : %d\nElapsed time : %.3lfs", move, elapsedTime/(double)1000);
		myFont.drawString(str, 15, ofGetHeight() - 20 - myFont.getLineHeight());

		// 무한 모드일 때는 여태까지 찾은 블럭들의 개수 표시
		if (infiniteMode) {
			ofSetColor(ofColor::red);
			sprintf(str, "Infinite Mode");
			myFont.drawString(str, 15, ofGetHeight() - 20 - myFont.getLineHeight()*3);
			sprintf(str, "%d block(s) found", blockFound);
			myFont.drawString(str, 15, ofGetHeight() - 20 - myFont.getLineHeight()*2);
		}
	}

	// 초기 안내 메시지
	if (!isPlaying && !isWon) {

		sprintf(str, "Escape the maze!\nYou win if you find red block.\nControl : Arrow Keys\nHint : h\nOn the lights: l\nAdjust light size : [ and ]\nGive up : g\nInfinite Mode : i\n\nPress n to Start New Game");

		auto lines = ofSplitString(str, "\n");
		float y = (ofGetHeight() - messageFont.stringHeight(str)) / 2 + messageFont.getAscenderHeight();

		for (auto line : lines) {
			messageFont.drawString(line, ofGetWindowWidth() / 2 - messageFont.stringWidth(line) / 2, y);

			y += messageFont.getLineHeight();
		
		}
	}

	// 승리 시 메시지
	if (isWon) {

		ofSetColor(ofColor::gold);

		sprintf(str, "You Win!\nBlocks found : %d\nTime : %.3lfs\nPress n to Start New Game", blockFound, elapsedTime / (double)1000);
		
		auto lines = ofSplitString(str, "\n");
		float y = (ofGetHeight() - messageFont.stringHeight(str)) / 2 + messageFont.getAscenderHeight();

		for (auto line : lines) {
			messageFont.drawString(line, ofGetWindowWidth() / 2 - messageFont.stringWidth(line) / 2, y);

			y += messageFont.getLineHeight();

		}
	}

	// 패배 시 메시지
	if (isGiveup) {

		ofSetColor(ofColor::red);

		sprintf(str, "You Lose!\nBlocks founded : %d\nTime : %.3lfs\nPress n to Start New Game", blockFound, elapsedTime / (double)1000);

		auto lines = ofSplitString(str, "\n");
		float y = (ofGetHeight() - messageFont.stringHeight(str)) / 2 + messageFont.getAscenderHeight();

		for (auto line : lines) {
			messageFont.drawString(line, ofGetWindowWidth() / 2 - messageFont.stringWidth(line) / 2, y);

			y += messageFont.getLineHeight();

		}
	}

} // end Draw


void ofApp::doFullScreen(bool bFull)
{
	// Enter full screen
	if (bFull) {
		// Remove the menu but don't destroy it
		menu->RemoveWindowMenu();
		// hide the cursor
		ofHideCursor();
		// Set full screen
		ofSetFullscreen(true);
	}
	else {
		// return from full screen
		ofSetFullscreen(false);
		// Restore the menu
		menu->SetWindowMenu();
		// Restore the window size allowing for the menu
		ofSetWindowShape(windowWidth, windowHeight + GetSystemMetrics(SM_CYMENU));
		// Centre on the screen
		ofSetWindowPosition((ofGetScreenWidth() - ofGetWidth()) / 2, (ofGetScreenHeight() - ofGetHeight()) / 2);
		// Show the cursor again
		ofShowCursor();
	}

} // end doFullScreen


//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	// Escape key exit has been disabled but it can be checked here
	if (key == VK_ESCAPE) {
		// Disable fullscreen set, otherwise quit the application as usual
		if (bFullscreen) {
			bFullscreen = false;
			doFullScreen(false);
		}
		else {
			ofExit();
		}
	}

	// 새 게임
	if (key == 'n') {
		loadGame();
	}

	// 플레이 중일 때만 플레이어 이동 및 각종 기능 활성화 가능
	if (isPlaying && !isWon && !isGiveup) {

		bool playerMoved = false;

		if (key == OF_KEY_LEFT && playerX > 0 && !(maze[playerY*MAZE_WIDTH + playerX] & WALL_LEFT)) {
			playerX--;
			playerMoved = true;
		}

		if (key == OF_KEY_RIGHT && playerX < MAZE_WIDTH - 1 && !(maze[playerY*MAZE_WIDTH + playerX] & WALL_RIGHT)) {
			playerX++;
			playerMoved = true;
		}

		if (key == OF_KEY_UP && playerY > 0 && !(maze[playerY*MAZE_WIDTH + playerX] & WALL_UP)) {
			playerY--;
			playerMoved = true;
		}

		if (key == OF_KEY_DOWN && playerY < MAZE_HEIGHT - 1 && !(maze[playerY*MAZE_WIDTH + playerX] & WALL_DOWN)) {
			playerY++;
			playerMoved = true;
		}

		// 플레이어 이동 시에 탈출 경로 갱신
		// 게임 종료 여부 체크
		if (playerMoved) {
			DFS();
			move++;
			gameFinishCheck();
		}

		// 힌트(탈출경로) 표시 기능
		if (key == 'h') {
			showHint = true;
		}

		// focus light 사이즈 조절 기능
		if (key == ']') {
			lightIncrement++;
		}

		if (key == '[') {
			lightIncrement--;
		}

		// focus light toggle
		if (key == 'l') {
			isLight = false;
		}

		// 게임 포기
		if (key == 'g') {
			if (isPlaying) {
				isGiveup = true;
			}
		}
	}

	// infinite 모드 토글
	if (key == 'i') {
		infiniteMode = !infiniteMode;
		menu->SetPopupItem("Infinite Mode", infiniteMode);
	}

	// full screen
	if (key == 'f') {
		bFullscreen = !bFullscreen;
		doFullScreen(bFullscreen);
	}

} // end keyPressed

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

	if (key == 'h') {
		showHint = false;
	}

	if (key == 'l') {
		isLight = true;
	}

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

void ofApp::gameFinishCheck() {
	// 도착지 도잘 여부 체크
	if (playerY == goalY && playerX == goalX) {
		blockFound++;
		// 무한 모드 일경우 reset=false로 하여 게임 재로드
		if (infiniteMode) {
			loadGame(false);
		}
		// 그렇지 않은 경우 isWon flag true로 설정
		else {
			isWon = true;
		}
	}

}

// 랜덤 숫자를 받아오는 함수
int ofApp::getRandNum(int min, int max) {
	thread_local std::random_device rd;
	thread_local std::mt19937 rng(rd());
	thread_local std::uniform_int_distribution<int> uid;
	return uid(rng, decltype(uid)::param_type{ min,max });
}

// 랜덤 불 값을 받아오는 함수
int ofApp::getRandBool() {
	return getRandNum(0, 1);
}

// 미로 생성 함수
void ofApp::createMaze(int h, int w, int* maze, pair<int, int> start) {

	// 스택
	stack<pair<int, int>> dfsStack;
	// 방문 배열
	vector<int> v(h*w, 0);
	// 미방문 연결 vertex 체크를 위한 벡터
	vector<int> m;


	// 상하좌워 체크를 위한 배열
	int di[4] = { 1 , -1, 0, 0 };
	int dj[4] = { 0, 0, 1, -1 };
	// 각 방향과 연관된 벽
	int dw[4] = { WALL_DOWN, WALL_UP, WALL_RIGHT, WALL_LEFT };
	int dnw[4] = { WALL_UP, WALL_DOWN, WALL_LEFT, WALL_RIGHT };

	// 시작점 push 및 방문 표시
	dfsStack.push(start);
	v[start.first*w + start.second] = 1;

	// 스택이 빌 때 까지
	while (!dfsStack.empty()) {

		auto top = dfsStack.top();

		int i = top.first;
		int j = top.second;

		// 미방문 연결 vertex 체크
		for (int k = 0; k < 4; k++) {

			int ni = i + di[k];
			int nj = j + dj[k];

			if (ni < 0 || ni >= h || nj < 0 || nj >= w) continue;
			if (v[ni*w + nj] > 0) continue;

			m.push_back(k);
		}

		// 미방문 연결 vertex가 없다면 pop
		if (m.empty()) {
			dfsStack.pop();
			continue;
		}

		// 미방문 연결 vertex가 있다면 랜덤으로 한 vertex를 선택하고
		// stack에 push함
		int s = getRandNum(0, m.size()-1);
		int ni = i + di[m[s]];
		int nj = j + dj[m[s]];

		v[ni*w + nj] = 1;
		maze[i * w + j] &= (WALL_ALL ^ dw[m[s]]);
		maze[ni * w + nj] &= (WALL_ALL ^ dnw[m[s]]);

		m.clear();
		dfsStack.push({ ni, nj });

	}

}

// 게임 로드
// reset이 false일 경우에는 게임 실행 관련 변수들은 초기화하지 않음
void ofApp::loadGame(bool reset) {

	// 각종 변수 초기화 
	if (reset) {
		blockFound = 0;
		move = 0;
		maze = new int[MAZE_HEIGHT*MAZE_WIDTH];
		route = new int[MAZE_HEIGHT*MAZE_WIDTH];
	}

	maze_size = (min(ofGetHeight(), ofGetWidth()) - MAZE_OFFSET) / max(MAZE_WIDTH, MAZE_HEIGHT);

	isWon = false;
	isGiveup = false;
	isLight = true;
	lightRadius = maze_size;
	lightIncrement = 0;

	for (int i = 0; i < MAZE_HEIGHT*MAZE_WIDTH; i++) {
		maze[i] = WALL_ALL;
	}
	memset(route, 0, sizeof(int) * MAZE_HEIGHT*MAZE_WIDTH);

	// 임의 시작 위치 설정
	if (reset) {
		int init = getRandNum(0, MAZE_HEIGHT*MAZE_WIDTH - 1);

		initX = init / MAZE_WIDTH;
		initY = init % MAZE_HEIGHT;
	}
	// infinite 모드인 경우 현재 플레이어의 위치가 시작위치
	else {
		initX = playerX;
		initY = playerY;
	}

	playerX = initX;
	playerY = initY;

	// 미로 생성
	createMaze(MAZE_HEIGHT, MAZE_WIDTH, maze, {initY, initX});

	// 도착지 설정
	setGoal();

	// 탈출 경로 계산
	DFS();

	isPlaying = true;

	// 게임 타이머 초기화
	if (reset) {
		game_clock = chrono::steady_clock::now();
	}

}

void ofApp::freeMemory() {

	// 게임 중인 경우에만 memory free
	if (isPlaying) {
		free(route);
		free(maze);
	}
}

// 도착지 설정 함수
void ofApp::setGoal() {

	// DFS 탐색 스택
	stack<pair<int, int>> dfsStack;
	// 초기 위치 삽입
	dfsStack.push({ initY, initX });

	// 방문 벡터
	vector<int> v(MAZE_HEIGHT*MAZE_WIDTH, 0);
	// 초기 위치 방문
	v[initY*MAZE_WIDTH + initX] = 1;

	// 스택이 빌 때 까지
	while (!dfsStack.empty()) {

		auto top = dfsStack.top();
		dfsStack.pop();

		int i = top.first;
		int j = top.second;

		int di[4] = { -1, 0, 1, 0 };
		int dj[4] = { 0, -1, 0, 1 };
		int dw[4] = { WALL_UP, WALL_LEFT, WALL_DOWN, WALL_RIGHT };

		// 각 방향에 대하여
		// 미방문 연결 지점 stack에 push
		for (int k = 0; k < 4; k++) {
			int ni = i + di[k];
			int nj = j + dj[k];

			if (ni < 0 || nj < 0 || ni >= MAZE_HEIGHT || nj >= MAZE_WIDTH || maze[i * MAZE_WIDTH + j] & dw[k]) {
				continue;
			}

			if (v[ni * MAZE_WIDTH + nj] > 0) {
				continue;
			}
			
			// 방문 배열의 값은 현재 방문 배열의 값 + 1
			dfsStack.push({ ni, nj });
			v[ni*MAZE_WIDTH + nj] = v[i*MAZE_WIDTH + j] + 1;

		}
	}

	// 방문 배열의 값이 최대인 좌표를 도착지로 설정
	int max_v = 0;
	int max_x = playerX;
	int max_y = playerY;

	for (int i = 0; i < v.size(); i++) {
		if (v[i] < max_v) continue;
		max_v = v[i];
		max_x = i % MAZE_WIDTH;
		max_y = i / MAZE_WIDTH;
	}

	goalY = max_y;
	goalX = max_x;
}

// 탈출 경로 계산 함수
bool ofApp::DFS()
{
	// 탈출 경로 초기화
	for (int i = 0; i < MAZE_HEIGHT * MAZE_WIDTH; i++) {
		route[i] = 0;
	}

	// dfs 스택 
	stack<pair<int, int>> dfsStack;
	dfsStack.push({ playerY, playerX });

	// 탈출 경로 스택
	stack<pair<int, int>> escapeStack;

	// dfs 스택이 빌 때 까지
	while (!dfsStack.empty()) {

		auto top = dfsStack.top();
		dfsStack.pop();

		int i = top.first;
		int j = top.second;

		// 방문된 정점이라면 건너뛰기
		if (route[i * MAZE_WIDTH + j] & ROUTE_VISITED) {
			continue;
		}

		// 탈출 경로 스택에 push
		escapeStack.push(top);

		// 방문 표시
		route[i * MAZE_WIDTH + j] |= ROUTE_VISITED;
		
		// 도착지라면 종료
		if (i == goalY && j == goalX) break;

		int originalSize = dfsStack.size();

		// 각 방향에 대하여
		// 미방문 연결 정점을 dfs 스택에 푸시
		int di[4] = { -1, 0, 1, 0 };
		int dj[4] = { 0, -1, 0, 1 };
		int dw[4] = { WALL_UP, WALL_LEFT, WALL_DOWN, WALL_RIGHT };

		for (int k = 0; k < 4; k++) {
			int ni = i + di[k];
			int nj = j + dj[k];

			if (!(maze[i * MAZE_WIDTH + j] & dw[k]) && !(route[ni * MAZE_WIDTH + nj] & ROUTE_VISITED)) {
				dfsStack.push({ ni, nj });
			}		
		}
		
		// 미방문 연결 정점이 없다면
		// 미로의 막다른 길이므로
		if (originalSize == dfsStack.size() && !dfsStack.empty()) {

			top = dfsStack.top();

			i = top.first;
			j = top.second;

			// 탈출 경로 스택이 빌 때 까지
			while (!escapeStack.empty()) {

				auto rTop = escapeStack.top();
				int ri = rTop.first;
				int rj = rTop.second;

				// dfs stack의 top-1 정점과 연결된 정점을 만날 때 까지 pop
				bool isAdjacent = false;
				int di[4] = { 1 , -1, 0, 0 };
				int dj[4] = { 0, 0, 1, -1 };
				int dw[4] = { WALL_DOWN, WALL_UP, WALL_RIGHT, WALL_LEFT };

				for (int k = 0; k < 4; k++) {
					if (i == ri + di[k] && j == rj + dj[k] && !(maze[ri * MAZE_WIDTH + rj] & dw[k])) {
						isAdjacent = true;
						break;
					}
				}

				// dfs 스택의 top-1 정점을 만날 수 있으면 break
				if (isAdjacent) break;

				escapeStack.pop();
			}

		}
	}

	// 탈출 경로 스택의 정점들을 탈출 경로로 표시
	while (!escapeStack.empty()) {
		auto top = escapeStack.top();
		escapeStack.pop();

		int i = top.first;
		int j = top.second;

		route[i * MAZE_WIDTH + j] |= ROUTE_ESCAPE;
	}	

	return 1;
}

// 탈출 경로를 그리는 함수
void ofApp::drawRoute() {
	// 각 칸에 대하여
	for (int i = 0; i < MAZE_HEIGHT; i++) {
		for (int j = 0; j < MAZE_WIDTH; j++) {

			// 각 방향에 대하여
			// 탈출 경로인 칸의 중심을 이어 탈출 경로 표시
			int di[4] = { 1 , -1, 0, 0 };
			int dj[4] = { 0, 0, 1, -1 };
			int dw[4] = { WALL_DOWN, WALL_UP, WALL_RIGHT, WALL_LEFT };

			int cx = j * maze_size + maze_size / 2;
			int cy = i * maze_size + maze_size / 2;

			for (int k = 0; k < 4; k++) {

				int ni = i + di[k];
				int nj = j + dj[k];

				if (ni < 0 || ni >= MAZE_HEIGHT || nj < 0 || nj >= MAZE_WIDTH || maze[i * MAZE_WIDTH + j] & dw[k]) continue;
				if (route[i * MAZE_WIDTH + j] & ROUTE_ESCAPE && route[ni * MAZE_WIDTH + nj] & ROUTE_ESCAPE) {

					int dx = nj * maze_size + maze_size / 2;
					int dy = ni * maze_size + maze_size / 2;

					ofSetColor(escapeRouteColor);

					ofDrawLine(x_offset + cx, y_offset + cy, x_offset + dx, y_offset + dy);
				}

			}
		}
	}
}