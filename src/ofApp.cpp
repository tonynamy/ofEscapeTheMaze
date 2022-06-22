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
	// Get the window size for image loading
	windowWidth = ofGetWidth();
	windowHeight = ofGetHeight();
	isOpen = 0;

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
	// Create a menu using ofxWinMenu
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
	menu->AddPopupItem(hPopup, "New Game", false, false); // Not checked and not auto-checked

	bShowInfo = true;  // screen info display on
	bTopmost = false; // app is topmost
	bFullscreen = false; // not fullscreen yet
	menu->AddPopupItem(hPopup, "Full screen", false, false); // Not checked and not auto-check

	menu->AddPopupItem(hPopup, "Game Setting", false, false); // Not checked and not auto-check

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
	dfsRouteColor = ofColor(236, 152, 196);
	bfsRouteColor = ofColor(200, 111, 215);

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
		isOpen = 0;
		freeMemory();
		ofExit(); // Quit the application
	}

	if (title == "Game Setting") {
		string a = ofSystemTextBoxDialog("Size of Maze?", to_string(MAZE_HEIGHT));
		int size = std::stoi(a);

		isPlaying = false;
		isWon = false;

		freeMemory();

		MAZE_HEIGHT = size;
		MAZE_WIDTH = size;
	}

	//
	// Window menu
	//

	if (title == "Full screen") {
		bFullscreen = !bFullscreen; // Not auto-checked and also used in the keyPressed function
		doFullScreen(bFullscreen); // But als take action immediately
	}

	//
	// Help menu
	//
	if (title == "About") {
		ofSystemAlertDialog("Escape the maze! You win if you find red block.\nControl : Arrow Keys\nHint : h\nOn the lights: l\nAdjust light size : [ and ]\nMade by Sogang Univ. tonynamy@apperz.co.kr");
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
	int i, j;

	// TO DO : DRAW MAZE; 
	// add code here

	if (isPlaying) {

		maze_size = (min(ofGetHeight(), ofGetWidth()) - MAZE_OFFSET) / max(MAZE_WIDTH, MAZE_HEIGHT);
		lightRadius = maze_size;
		x_offset = (ofGetWidth() - maze_size * MAZE_WIDTH) / 2;
		y_offset = (ofGetHeight() - maze_size * MAZE_HEIGHT) / 2;

		for (int i = 0; i < MAZE_HEIGHT; i++) {
			for (int j = 0; j < MAZE_WIDTH; j++) {

				if (maze[i * MAZE_WIDTH + j] & WALL_UP) ofDrawLine(x_offset+j * maze_size, y_offset+i * maze_size, x_offset+(j + 1) * maze_size, y_offset + i * maze_size);
				if (maze[i * MAZE_WIDTH + j] & WALL_DOWN) ofDrawLine(x_offset+j * maze_size, y_offset + (i + 1) * maze_size, x_offset+(j + 1) * maze_size, y_offset + (i + 1) * maze_size);
				if (maze[i * MAZE_WIDTH + j] & WALL_LEFT) ofDrawLine(x_offset+j * maze_size, y_offset + i * maze_size, x_offset+j * maze_size, y_offset + (i + 1) * maze_size);
				if (maze[i * MAZE_WIDTH + j] & WALL_RIGHT) ofDrawLine(x_offset+(j + 1) * maze_size, y_offset + i * maze_size, x_offset+(j + 1) * maze_size, y_offset + (i + 1) * maze_size);

			}
		}

		if (showHint) {
			drawRoute();
		}

		int cx = playerX * maze_size + maze_size / 2 + x_offset;
		int cy = playerY * maze_size + maze_size / 2 + y_offset;

		int gx = goalX * maze_size + maze_size / 2 + x_offset;
		int gy = goalY * maze_size + maze_size / 2 + y_offset;

		int player_size = maze_size / 3;

		ofSetRectMode(OF_RECTMODE_CENTER);
		ofDrawRectangle(cx, cy, player_size, player_size);

		ofSetColor(ofColor::red);
		ofDrawRectangle(gx, gy, player_size, player_size);

		if (isLight && !isWon) {

			fbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);

			fbo.begin();
			ofClear(ofColor::black);

			int radiusStepSize = 3;  // Decrease for more circles (i.e. a more opaque brush)
			int alpha = 3;  // Increase for a more opaque brush
			int maxOffsetDistance = 100;  // Increase for a larger spread of circles

			auto a = ofColor(255, 255, 255, 255);
			auto b = ofColor(255, 255, 255, 100);

			ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);

			for (int radius = lightRadius + lightIncrement; radius > 0; radius -= radiusStepSize) {

				ofSetColor(a.lerp(b, (lightRadius + lightIncrement) / radius));

				// Using the ofColor class, we will randomly select a color between orange and red

				ofDrawCircle(cx, cy, radius);

			}

			ofDisableBlendMode();

			fbo.end();

			ofSetRectMode(OF_RECTMODE_CORNER);

			fbo.draw(0.f, 0.f);
		}

	}

	if (isPlaying && !isWon) {
		elapsedTime = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - game_clock).count();
	}

	if (isPlaying) {
		ofSetColor(100);
		// Show keyboard duplicates of menu functions
		sprintf(str, "Elapsed time : %.3lfs", elapsedTime/(double)1000);
		myFont.drawString(str, 15, ofGetHeight() - 20);
	}

	if (!isPlaying && !isWon) {

		sprintf(str, "Press n to Start New Game");

		auto lines = ofSplitString(str, "\n");
		float y = (ofGetHeight() - messageFont.stringHeight(str)) / 2 + messageFont.getAscenderHeight();

		for (auto line : lines) {
			messageFont.drawString(line, ofGetWindowWidth() / 2 - messageFont.stringWidth(line) / 2, y);

			y += messageFont.getLineHeight();
		
		}
	}

	if (isWon) {

		sprintf(str, "You Won!!\nTime : %.3lfs\nPress n to Start New Game", elapsedTime / (double)1000);
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
		// Restore topmost state
		if (bTopmost) doTopmost(true);
	}

} // end doFullScreen


void ofApp::doTopmost(bool bTop)
{
	if (bTop) {
		// get the current top window for return
		hWndForeground = GetForegroundWindow();
		// Set this window topmost
		SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
	}
	else {
		SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ShowWindow(hWnd, SW_SHOW);
		// Reset the window that was topmost before
		if (GetWindowLong(hWndForeground, GWL_EXSTYLE) & WS_EX_TOPMOST)
			SetWindowPos(hWndForeground, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		else
			SetWindowPos(hWndForeground, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
} // end doTopmost


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

	if (key == 'n') {
		loadGame();
	}

	if (isPlaying && !isWon) {

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

		if (playerMoved) {
			DFS();
			gameFinishCheck();
		}

		if (key == 'h') {
			showHint = true;
		}

		if (key == ']') {
			lightIncrement++;
		}

		if (key == '[') {
			lightIncrement--;
		}

		if (key == 'l') {
			isLight = false;
		}
	}

	// Remove or show screen info
	if (key == ' ') {
		bShowInfo = !bShowInfo;
		// Update the menu check mark because the item state has been changed here
		menu->SetPopupItem("Show DFS", bShowInfo);
	}

	if (key == 'f') {
		bFullscreen = !bFullscreen;
		doFullScreen(bFullscreen);
		// Do not check this menu item
		// If there is no menu when you call the SetPopupItem function it will crash
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

	if (playerY == goalY && playerX == goalX) {
		isWon = true;
	}

}

int ofApp::getRandNum(int min, int max) {
	static thread_local std::mt19937 generator(time(NULL));
	std::uniform_int_distribution<int> distribution(min, max);
	return distribution(generator);
}

int ofApp::getRandBool(int seed, int i, int j, int w) {
	//srand(seed + i * w + j);
	return getRandNum(0, 1);
}

void ofApp::createMaze(int i, int h, int w, int* maze, int* maze_set, int seed) {

	for (int j = 0; j < w - 1; j++) {
		if (abs(maze_set[i * w + j]) != abs(maze_set[i * w + j + 1]) && (i == h - 1 || getRandBool(seed, i, j, w))) {
			maze[i * w + j] &= (WALL_ALL ^ WALL_RIGHT);
			maze[i * w + j + 1] &= (WALL_ALL ^ WALL_LEFT);
			
			int change_maze_set = maze_set[i * w + j + 1];
			for (int k = 0; k < w; k++) {
				if (maze_set[i*w + k] == change_maze_set) {
					maze_set[i*w + k] = maze_set[i * w + j];
				}
			}
			
		}
	}

	if (i == h - 1) return;

	for (int j = 0; j < w; j++) {

		if (maze_set[i * w + j] < 0) continue;

		int set = maze_set[i * w + j];
		maze_set[i * w + j] = -set;

		int last_index = j;
		int flag = 0;

		for (int jj = j + 1; jj < w; jj++) {

			if (maze_set[i * w + jj] == set) {
				maze_set[i * w + jj] = -set;
				last_index = jj;

				if (getRandBool(seed, i, jj, w)) {
					maze[i * w + jj] &= (WALL_ALL ^ WALL_DOWN);
					maze[(i + 1) * w + jj] &= (WALL_ALL ^ WALL_UP);
					maze_set[(i + 1) * w + jj] = set;
					flag = 1;
				}

			}

		}

		if (!flag) {
			maze[i * w + last_index] &= (WALL_ALL ^ WALL_DOWN);
			maze[(i + 1) * w + last_index] &= (WALL_ALL ^ WALL_UP);
			maze_set[(i + 1) * w + last_index] = set;
		}

	}

}

void ofApp::loadGame() {

	maze = new int[MAZE_HEIGHT*MAZE_WIDTH];
	route = new int[MAZE_HEIGHT*MAZE_WIDTH];

	maze_size = (min(ofGetHeight(), ofGetWidth()) - MAZE_OFFSET) / max(MAZE_WIDTH, MAZE_HEIGHT);

	isWon = false;
	isLight = true;
	lightRadius = maze_size;
	lightIncrement = 0;

	int* maze_set = new int[MAZE_HEIGHT*MAZE_WIDTH];

	for (int i = 0; i < MAZE_HEIGHT*MAZE_WIDTH; i++) {
		maze[i] = WALL_ALL;
	}
	memset(route, 0, sizeof(int) * MAZE_HEIGHT*MAZE_WIDTH);

	int seed = time(NULL);

	int set = 0;

	for (int i = 0; i < MAZE_HEIGHT; i++) {
		for (int j = 0; j < MAZE_WIDTH; j++) {
			if (maze_set[i * MAZE_WIDTH + j] > 0) continue;
			maze_set[i * MAZE_WIDTH + j] = set++;
		}
		createMaze(i, MAZE_HEIGHT, MAZE_WIDTH, maze, maze_set, seed);
	}

	delete maze_set;

	int init = getRandNum(0, MAZE_HEIGHT*MAZE_WIDTH-1);

	initY = init / MAZE_WIDTH;
	initX = init % MAZE_HEIGHT;

	playerX = initX;
	playerY = initY;

	setGoal();

	DFS();

	isPlaying = true;

	game_clock = chrono::steady_clock::now();

}

void ofApp::freeMemory() {

	//TO DO
	if (isPlaying) {
		free(route);
		free(maze);
	}
}

void ofApp::setGoal() {

	// DFS Ž�� ����
	stack<pair<int, int>> dfsStack;
	dfsStack.push({ initY, initX });

	// �湮 ����
	vector<int> v(MAZE_HEIGHT*MAZE_WIDTH, 0);
	v[initY*MAZE_WIDTH + initX] = 1;

	while (!dfsStack.empty()) {

		auto top = dfsStack.top();
		dfsStack.pop();

		int i = top.first;
		int j = top.second;

		int di[4] = { -1, 0, 1, 0 };
		int dj[4] = { 0, -1, 0, 1 };
		int dw[4] = { WALL_UP, WALL_LEFT, WALL_DOWN, WALL_RIGHT };

		for (int k = 0; k < 4; k++) {
			int ni = i + di[k];
			int nj = j + dj[k];

			if (ni < 0 || nj < 0 || ni >= MAZE_HEIGHT || nj >= MAZE_WIDTH || maze[i * MAZE_WIDTH + j] & dw[k]) {
				continue;
			}

			if (v[ni * MAZE_WIDTH + nj] > 0) {
				continue;
			}
			
			dfsStack.push({ ni, nj });
			v[ni*MAZE_WIDTH + nj] = v[i*MAZE_WIDTH + j] + 1;

		}
	}

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

bool ofApp::DFS()
{
	// Ż�� ��� �ʱ�ȭ
	for (int i = 0; i < MAZE_HEIGHT * MAZE_WIDTH; i++) {
		route[i] = 0;
	}

	stack<pair<int, int>> dfsStack;
	dfsStack.push({ playerY, playerX });

	stack<pair<int, int>> escapeStack;

	while (!dfsStack.empty()) {

		auto top = dfsStack.top();
		dfsStack.pop();

		int i = top.first;
		int j = top.second;

		if (route[i * MAZE_WIDTH + j] & ROUTE_DFS) {
			continue;
		}

		escapeStack.push(top);

		route[i * MAZE_WIDTH + j] |= ROUTE_DFS;

		if (i == goalY && j == goalX) break;

		int originalSize = dfsStack.size();

		int di[4] = { -1, 0, 1, 0 };
		int dj[4] = { 0, -1, 0, 1 };
		int dw[4] = { WALL_UP, WALL_LEFT, WALL_DOWN, WALL_RIGHT };

		for (int k = 0; k < 4; k++) {
			int ni = i + di[k];
			int nj = j + dj[k];

			if (!(maze[i * MAZE_WIDTH + j] & dw[k]) && !(route[ni * MAZE_WIDTH + nj] & ROUTE_DFS)) {
				dfsStack.push({ ni, nj });
			}		
		}
		
		if (originalSize == dfsStack.size() && !dfsStack.empty()) {

			top = dfsStack.top();

			i = top.first;
			j = top.second;

			while (!escapeStack.empty()) {

				auto rTop = escapeStack.top();
				int ri = rTop.first;
				int rj = rTop.second;

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

				if (isAdjacent) break;

				escapeStack.pop();
			}

		}
	}

	while (!escapeStack.empty()) {
		auto top = escapeStack.top();
		escapeStack.pop();

		int i = top.first;
		int j = top.second;

		route[i * MAZE_WIDTH + j] |= ROUTE_ESCAPE;
	}	

	return 1;
}

void ofApp::drawRoute() {
	//TO DO 
	for (int i = 0; i < MAZE_HEIGHT; i++) {
		for (int j = 0; j < MAZE_WIDTH; j++) {

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