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
int r;
int l;
int *completed = 0;
unsigned char midi;
unsigned char vel;
unsigned char buffer[64];
unsigned char rec[64];
unsigned char all_light_off[] = { 0x13, 0xF0, 0x00, 0x20, 0x13, 0x29, 0x02, 0x10, 0x17, 0x0E, 0x00, 0xF7 };


static void LIBUSB_CALL cb_in(struct libusb_transfer *receive)
{

	if (receive->status == LIBUSB_TRANSFER_COMPLETED)
	{
		for (int i = 0; i < sizeof(receive->buffer); i++) {
			printf("%0x", receive->buffer[i]);
		}
		printf("\n");

		libusb_submit_transfer(receive);
	}
}
//
// static void LIBUSB_CALL cb_out(struct libusb_transfer *send) {
//	unsigned char data9[] = { 0x13, 0xF0, 0x00, 0x20, 0x13, 0x29, 0x02, 0x10, 0x13, 0x0A, 0x51, 0x55, 0x15, 0xF7 };
//	r = libusb_bulk_transfer(devh, 1, data9, 64, &actual, 0);
//	printf("SEND function:, %s",libusb_error_name(r));
//	printf("\n");
//	libusb_submit_transfer(send);
//}

void get() {
	r = libusb_bulk_transfer(devh, 129, buffer, sizeof(buffer), &actual, 0);

	for (int i = 0; i < actual; i++) {
		printf("%0x", buffer[i]);
	}
	printf("\n");


	midi = buffer[2];
	vel = buffer[3];

	unsigned char light_on[] = { 0x19, 0x90, midi, 0x2d };
	unsigned char light_off[] = { 0x19, 0x90, midi, 0x00 };

	if (vel > 55) {
		r = libusb_bulk_transfer(devh, 1, light_on, sizeof(light_on), &actual, 0);
	}

	if (vel < 55) {
		r = libusb_bulk_transfer(devh, 1, light_off, sizeof(light_off), &actual, 0);
	}
	memset(buffer, 0, 64);
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
	Sleep(1000);
	//-------------------------------------------------------------------------------------------
	/*unsigned char data9[] = { 0x13, 0xF0, 0x00, 0x20, 0x13, 0x29, 0x02, 0x10, 0x13, 0x0A, 0x51, 0x55, 0x15, 0xF7 };
	r = libusb_bulk_transfer(devh, 1, data9, 64, &actual, 0);*/

	r = libusb_bulk_transfer(devh, 1, all_light_off, sizeof(all_light_off), &actual, 0);

	while (!completed) {
		get();
	}

	//-----------------------------------------------------------
	//struct libusb_transfer *send;
	//send = libusb_alloc_transfer(0);
	//unsigned char dataOut[64];

	//libusb_fill_bulk_transfer(send,
	//	devh,
	//	0x1, // Endpoint ID
	//	dataOut,
	//	64,
	//	cb_out,
	//	NULL,
	//	0
	//);

	//if (libusb_submit_transfer(send) < 0)
	//{
	//	// Error
	//	libusb_free_transfer(send);
	//	free(dataOut);
	//}

//-------------------------------------------------------------
	//struct libusb_transfer *receive;
	//receive = libusb_alloc_transfer(0);
	//unsigned char data[64];

	//libusb_fill_bulk_transfer(receive,
	//	devh,
	//	0x81, // Endpoint ID
	//	buffer,
	//	sizeof(buffer),
	//	cb_in,
	//	data,
	//	0
	//);

	//if (libusb_submit_transfer(receive) < 0)
	//{
	//	// error
	//	libusb_free_transfer(receive);
	//	free(data);
	//}

	//while (!completed)
	//{
	//	libusb_handle_events(ctx);
	//}


	//r = libusb_set_interface_alt_setting(devh, 1, 0);


	//Sleep(3000);
	//r = libusb_interrupt_transfer(devh, 1, data1, 0, &actual, 0);

	//r = libusb_bulk_transfer(devh, 1, data1, 0, &actual, 10);
	

	//r = libusb_interrupt_transfer(devh, 1, data2, sizeof(data2), &actual, 0);

	/*cout<<buffer<<endl;
	cout << actual << endl;*/

	r = libusb_release_interface(devh, 1);
	if (r != 0) {
		return 7;
	}

	libusb_close(devh);
	libusb_exit(ctx);
	return 0;
}




