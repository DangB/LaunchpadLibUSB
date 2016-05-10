// Launchpad.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "libusb.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <thread>
#include <time.h>
#include <random>

using namespace std;

struct libusb_device_handle *devh = NULL;
static libusb_context *ctx = NULL;

int actual;
int actual2;
int r;
int l;
int *completed = 0;

std::default_random_engine rd1;
std::default_random_engine rd;
std::uniform_real_distribution<double> dist(2, 8);
std::uniform_real_distribution<double> dist1(5, 8);
std::uniform_real_distribution<double> dist2(0, 2);

unsigned char vel;
unsigned char buffer[64];
unsigned char all_light_off[] = { 0x13, 0xF0, 0x00, 0x20, 0x13, 0x29, 0x02, 0x10, 0x17, 0x0E, 0x00, 0xF7 };
unsigned char result[64];

INPUT iup;
INPUT ileft;
INPUT idown;
INPUT iright;

INPUT ilowP;
INPUT imedP;
INPUT ihighP;
INPUT icomboP;

INPUT ilowK;
INPUT imedK;
INPUT ihighK;
INPUT icomboK;

void get();
void init_layout();
void init_keys();
void keyPressDown(INPUT in);
void keyPressUp(INPUT in);
void keySwitchDown(unsigned char c);
void keySwitchUp(unsigned char c);
void crossLight(int n);
void plusLight(int color);
void threadPlusTask(int color);
void threadCrossTask(int color);
void directionLightOn(int num);
void directionLightOff(int num);

void init_layout() {
	unsigned char layout[64] = {0x13, 0xf0, 0x00, 0x20, 0x13, 0x29, 0x02, 0x10, 0x13, 0x0a, 
		32, 3, 0x13, 21, 3, 23, 0x13, 3, 22, 3, 
		0x13, 25, 5, 26, 0x13, 13, 27, 45, 0x13, 28, 21, 
		35, 0x13, 5, 36, 13, 0x13, 37, 45, 38, 0x16, 21, 
		0xf7};

	r = libusb_bulk_transfer(devh, 1, layout, sizeof(layout), &actual, 0);
}

void init_keys() {
	iup.type = INPUT_KEYBOARD;
	iup.ki.wScan = 0;
	iup.ki.time = 0;
	iup.ki.dwExtraInfo = 0;
	iup.ki.wVk = 'W';

	idown.type = INPUT_KEYBOARD;
	idown.ki.wScan = 0;
	idown.ki.time = 0;
	idown.ki.dwExtraInfo = 0;
	idown.ki.wVk = 'S';

	ileft.type = INPUT_KEYBOARD;
	ileft.ki.wScan = 0;
	ileft.ki.time = 0;
	ileft.ki.dwExtraInfo = 0;
	ileft.ki.wVk = 'A';

	iright.type = INPUT_KEYBOARD;
	iright.ki.wScan = 0;
	iright.ki.time = 0;
	iright.ki.dwExtraInfo = 0;
	iright.ki.wVk = 'D';
	//--------------------------------------------------
	ilowP.type = INPUT_KEYBOARD;
	ilowP.ki.wScan = 0;
	ilowP.ki.time = 0;
	ilowP.ki.dwExtraInfo = 0;
	ilowP.ki.wVk = 'G';

	imedP.type = INPUT_KEYBOARD;
	imedP.ki.wScan = 0;
	imedP.ki.time = 0;
	imedP.ki.dwExtraInfo = 0;
	imedP.ki.wVk = 'H';

	ihighP.type = INPUT_KEYBOARD;
	ihighP.ki.wScan = 0;
	ihighP.ki.time = 0;
	ihighP.ki.dwExtraInfo = 0;
	ihighP.ki.wVk = 'J';

	icomboP.type = INPUT_KEYBOARD;
	icomboP.ki.wScan = 0;
	icomboP.ki.time = 0;
	icomboP.ki.dwExtraInfo = 0;
	icomboP.ki.wVk = 'K';
	//------------------------------------------------------
	ilowK.type = INPUT_KEYBOARD;
	ilowK.ki.wScan = 0;
	ilowK.ki.time = 0;
	ilowK.ki.dwExtraInfo = 0;
	ilowK.ki.wVk = 'B';

	imedK.type = INPUT_KEYBOARD;
	imedK.ki.wScan = 0;
	imedK.ki.time = 0;
	imedK.ki.dwExtraInfo = 0;
	imedK.ki.wVk = 'N';

	ihighK.type = INPUT_KEYBOARD;
	ihighK.ki.wScan = 0;
	ihighK.ki.time = 0;
	ihighK.ki.dwExtraInfo = 0;
	ihighK.ki.wVk = 'M';

	icomboK.type = INPUT_KEYBOARD;
	icomboK.ki.wScan = 0;
	icomboK.ki.time = 0;
	icomboK.ki.dwExtraInfo = 0;
	icomboK.ki.wVk = VK_OEM_COMMA;
}

void keyPressDown(INPUT in) {
	in.ki.dwFlags = 0;
	SendInput(1, &in, sizeof(INPUT));
}

void keyPressUp(INPUT in) {
	in.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &in, sizeof(INPUT));
}

void crossLight(int color) {
	int padCol = 0;
	int padRow = 0;
	int padNum = 0;

	padCol = dist(rd1);
	padRow = dist1(rd1);

	printf("%d\n", padCol);
	printf("%d\n", padRow);
	padNum = (padRow * 10) + padCol;
	printf("%d\n", padNum);
	unsigned char cross[64] = { 0x13, 0xf0, 0x00, 0x20,
		0x13, 0x29, 0x02, 0x10,
		0x13, 0x28, padNum, color,
		0x13, padNum - 11, color, padNum + 11,
		0x13, color, padNum + 9, color,
		0x17, padNum - 9, color, 0xf7 };
	unsigned char offC[64] = { 0x13, 0xf0, 0x00, 0x20,
		0x13, 0x29, 0x02, 0x10,
		0x13, 0x0a, padNum, 0,
		0x13, padNum - 11, 0, padNum + 11,
		0x13, 0, padNum - 9, 0,
		0x17, padNum + 9, 0, 0xf7 };
	r = libusb_bulk_transfer(devh, 1, cross, sizeof(cross), &actual, 0);
	Sleep(200);
	r = libusb_bulk_transfer(devh, 1, offC, sizeof(offC), &actual, 0);
}

void plusLight(int color) {
	int padCol = 0;
	int padRow = 0;
	int padNum = 0;

	padCol = dist(rd);
	padRow = dist1(rd);

	printf("%d\n", padCol);
	printf("%d\n", padRow);
	padNum = (padRow*10) + padCol;
	printf("%d\n", padNum);
	unsigned char plus[64] = {0x13, 0xf0, 0x00, 0x20, 
								0x13, 0x29, 0x02, 0x10, 
								0x13, 0x28, padNum, color, 
								0x13, padNum-1, color, padNum+1, 
								0x13, color, padNum+10, color, 
								0x17, padNum-10, color, 0xf7};
	unsigned char offL[64] = { 0x13, 0xf0, 0x00, 0x20,
								0x13, 0x29, 0x02, 0x10,
								0x13, 0x0a, padNum, 0,
								0x13, padNum - 1, 0, padNum + 1,
								0x13, 0, padNum - 10, 0,
								0x17, padNum + 10, 0, 0xf7 };
	r = libusb_bulk_transfer(devh, 1, plus, sizeof(plus), &actual, 0);
	Sleep(200);
	r = libusb_bulk_transfer(devh, 1, offL, sizeof(offL), &actual, 0);
}

void directionLightOn(int num) {
	unsigned char upDirection[64] = {0x13, 0xf0, 0x00, 0x20, 
									0x13, 0x29, 0x02, 0x10, 
									0x13, 0x0a, 91, 3, 
									0x13, 92, 3, 93, 
									0x13, 3, 94, 3, 
									0x13, 95, 3, 96, 
									0x13, 3, 97, 3, 
									0x17, 98, 3, 0xf7};

	unsigned char downDirection[64] = { 0x13, 0xf0, 0x00, 0x20,
										0x13, 0x29, 0x02, 0x10,
										0x13, 0x0a, 1, 3,
										0x13, 2, 3, 3,
										0x13, 3, 4, 3,
										0x13, 5, 3, 6,
										0x13, 3, 7, 3,
										0x17, 8, 3, 0xf7 };

	unsigned char leftDirection[64] = { 0x13, 0xf0, 0x00, 0x20,
										0x13, 0x29, 0x02, 0x10,
										0x13, 0x0a, 10, 3,
										0x13, 20, 3, 30,
										0x13, 3, 40, 3,
										0x13, 50, 3, 60,
										0x13, 3, 70, 3,
										0x17, 80, 3, 0xf7 };

	unsigned char rightDirection[64] = { 0x13, 0xf0, 0x00, 0x20,
										0x13, 0x29, 0x02, 0x10,
										0x13, 0x0a, 19, 3,
										0x13, 29, 3, 39,
										0x13, 3, 49, 3,
										0x13, 59, 3, 69,
										0x13, 3, 79, 3,
										0x17, 89, 3, 0xf7 };

	switch (num) {
	case 0:
		r = libusb_bulk_transfer(devh, 1, upDirection, sizeof(upDirection), &actual, 0);
		break;
	case 1:
		r = libusb_bulk_transfer(devh, 1, downDirection, sizeof(downDirection), &actual, 0);
		break;
	case 2:
		r = libusb_bulk_transfer(devh, 1, leftDirection, sizeof(leftDirection), &actual, 0);
		break;
	case 3:
		r = libusb_bulk_transfer(devh, 1, rightDirection, sizeof(rightDirection), &actual, 0);
		break;
	}
}

void directionLightOff(int num) {
	unsigned char upDirection[64] = { 0x13, 0xf0, 0x00, 0x20,
		0x13, 0x29, 0x02, 0x10,
		0x13, 0x0a, 91, 0,
		0x13, 92, 0, 93,
		0x13, 0, 94, 0,
		0x13, 95, 0, 96,
		0x13, 0, 97, 0,
		0x17, 98, 0, 0xf7 };

	unsigned char downDirection[64] = { 0x13, 0xf0, 0x00, 0x20,
		0x13, 0x29, 0x02, 0x10,
		0x13, 0x0a, 1, 0,
		0x13, 2, 0, 3,
		0x13, 0, 4, 0,
		0x13, 5, 0, 6,
		0x13, 0, 7, 0,
		0x17, 8, 0, 0xf7 };

	unsigned char leftDirection[64] = { 0x13, 0xf0, 0x00, 0x20,
		0x13, 0x29, 0x02, 0x10,
		0x13, 0x0a, 10, 0,
		0x13, 20, 0, 30,
		0x13, 0, 40, 0,
		0x13, 50, 0, 60,
		0x13, 0, 70, 0,
		0x17, 80, 0, 0xf7 };

	unsigned char rightDirection[64] = { 0x13, 0xf0, 0x00, 0x20,
		0x13, 0x29, 0x02, 0x10,
		0x13, 0x0a, 19, 0,
		0x13, 29, 0, 39,
		0x13, 0, 49, 0,
		0x13, 59, 0, 69,
		0x13, 0, 79, 0,
		0x17, 89, 0, 0xf7 };

	switch (num) {
	case 0:
		r = libusb_bulk_transfer(devh, 1, upDirection, sizeof(upDirection), &actual, 0);
		break;
	case 1:
		r = libusb_bulk_transfer(devh, 1, downDirection, sizeof(downDirection), &actual, 0);
		break;
	case 2:
		r = libusb_bulk_transfer(devh, 1, leftDirection, sizeof(leftDirection), &actual, 0);
		break;
	case 3:
		r = libusb_bulk_transfer(devh, 1, rightDirection, sizeof(rightDirection), &actual, 0);
		break;
	}
}

void threadPlusTask(int color) {
	std::thread t(plusLight, color);
	t.detach();
}

void threadCrossTask(int color) {
	std::thread t(crossLight, color);
	t.detach();
}

void randomTask(int color) {
	int num = dist2(rd);
	printf("%d\n", num);
	switch (num) {
	case 0:
		threadPlusTask(color);
		break;
	case 1:
		threadCrossTask(color);
		break;
	}
}


void keySwitchDown(unsigned char c) {
	switch (c) {
	case 32:
		keyPressDown(iup);
		directionLightOn(0);
		break;
	case 22:
		keyPressDown(idown);
		directionLightOn(1);
		break;
	case 21:
		keyPressDown(ileft);
		directionLightOn(2);
		break;
	case 23:
		keyPressDown(iright);
		directionLightOn(3);
		break;
	case 35:
		keyPressDown(ilowP);
		randomTask(5);
		break;
	case 36:
		keyPressDown(imedP);
		randomTask(13);
		break;
	case 37:
		keyPressDown(ihighP);
		randomTask(45);
		break;
	case 38:
		keyPressDown(icomboP);
		randomTask(21);
		break;
	case 25:
		keyPressDown(ilowK);
		randomTask(5);
		break;
	case 26:
		keyPressDown(imedK);
		randomTask(13);
		break;
	case 27:
		keyPressDown(ihighK);
		randomTask(45);
		break;
	case 28:
		keyPressDown(icomboK);
		randomTask(21);
		break;
	default:
		break;
	}
}

void keySwitchUp(unsigned char c) {
	switch (c) {
	case 32:
		keyPressUp(iup);
		directionLightOff(0);
		break;
	case 22:
		keyPressUp(idown);
		directionLightOff(1);
		break;
	case 21:
		keyPressUp(ileft);
		directionLightOff(2);
		break;
	case 23:
		keyPressUp(iright);
		directionLightOff(3);
		break;
	case 35:
		keyPressUp(ilowP);
		break;
	case 36:
		keyPressUp(imedP);
		break;
	case 37:
		keyPressUp(ihighP);
		break;
	case 38:
		keyPressUp(icomboP);
		break;
	case 25:
		keyPressUp(ilowK);
		break;
	case 26:
		keyPressUp(imedK);
		break;
	case 27:
		keyPressUp(ihighK);
		break;
	case 28:
		keyPressUp(icomboK);
		break;
	default:
		break;
	}
}

void get(int t) {
	r = libusb_interrupt_transfer(devh, 0x81, buffer, sizeof(buffer), &actual, t);
	if (r == 0) {
		for (int i = 0; i < actual; i++) {
			printf("%0x", buffer[i]);
		}
		printf("\n");

		for (int i = 0; i < actual; i = i + 4) {
			for (int j = 0; j < 4; j++) {
				result[j] = buffer[j+i];
				printf("%0x", result[j]);
			}
			printf("\n");

			if (result[3]!=0) {
				keySwitchDown(result[2]);
			}
			else {
				keySwitchUp(result[2]);
			}

			memset(result, 0, 64);
		}
		

		//if (vel != 0) {
		//	for (int i = 3; i < actual; i = i + 4) {
		//		buffer[i] = 0x2d;
		//	}
		//	r = libusb_interrupt_transfer(devh, 1, buffer, sizeof(actual), &actual, 0);

		//}

		//if (vel == 0) {
		//	for (int i = 3; i < actual; i = i + 4) {
		//		buffer[i] = 0x00;
		//	}
		//	r = libusb_bulk_transfer(devh, 1, buffer, sizeof(actual), &actual, 0);
		//}
		memset(buffer, 0, 64);
	}

}


int main()
{
	int actual;

	r = libusb_init(NULL);
	if (r < 0) {
		return 1;
	}

	libusb_set_debug(ctx, 3);


	devh = libusb_open_device_with_vid_pid(ctx, 0x1235, 0x51);
	if (!devh) {
		return 2;
	}

	r = libusb_claim_interface(devh, 1);

	if (r < 0) {
		return 4;
	}

	//-------------------------------------------------------------------------------------------
	unsigned char init1[8];
	unsigned char init2[512];

	r = libusb_bulk_transfer(devh, 129, init1, sizeof(init1), &actual, 100);

	r = libusb_bulk_transfer(devh, 129, init2, sizeof(init2), &actual, 100);
	Sleep(100);
	//-------------------------------------------------------------------------------------------

	r = libusb_bulk_transfer(devh, 1, all_light_off, sizeof(all_light_off), &actual, 0);
	Sleep(100);

	init_layout();
	init_keys();

	while (!completed) {
		get(0);
	}

	r = libusb_release_interface(devh, 1);
	if (r != 0) {
		return 7;
	}

	libusb_close(devh);
	libusb_exit(ctx);
	return 0;
}
