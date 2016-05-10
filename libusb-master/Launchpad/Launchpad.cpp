// Launchpad.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "libusb.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

struct libusb_device_handle *devh = NULL;
static libusb_context *ctx = NULL;

int actual;
int actual2;
int r;
int l;
int *completed = 0;
unsigned char midi;
unsigned char vel;
unsigned char buffer[64];
unsigned char rec[64];
unsigned char all_light_off[] = { 0x13, 0xF0, 0x00, 0x20, 0x13, 0x29, 0x02, 0x10, 0x17, 0x0E, 0x00, 0xF7 };
unsigned char result[64];
void get();

void get() {

	r = libusb_bulk_transfer(devh, 0x81, buffer, sizeof(buffer), &actual, 0);
	if (r == 0) {
		for (int i = 0; i < actual; i++) {
			printf("%0x", buffer[i]);
		}
		printf("\n");

		for (int i = 0; i < actual; i = i + 4) {
			for (int j = 0; j < 4; j++) {
				result[j] = buffer[j + i];
			}
			if (result[3] != 0) {
				result[3] = 0x2d;
			}
			else {
				result[3] = 0;
			}
			r = libusb_interrupt_transfer(devh, 1, result, sizeof(4), &actual2, 0);
			memset(result, 0, 64);
		}
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

	r = libusb_bulk_transfer(devh, 1, all_light_off, sizeof(all_light_off), &actual, 0);
	Sleep(1000);
	//--------------------------------------------------------------------------------------------
	unsigned char set_all_led[64] = {0x14, 0xf0, 0x00, 0x20, 0x14, 0x29, 0x02, 0x10, 0x13, 0x0e, 45, 0xf7};
	r = libusb_bulk_transfer(devh, 1, set_all_led, sizeof(set_all_led), &actual, 0);


	//--------------------------------------------------------------------------------------------
	while (!completed) {
		get();
	}

	r = libusb_release_interface(devh, 1);
	if (r != 0) {
		return 7;
	}

	libusb_close(devh);
	libusb_exit(ctx);
	return 0;
}




