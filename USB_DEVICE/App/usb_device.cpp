/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : usb_device.c
 * @version        : v1.0_Cube
 * @brief          : This file implements the USB Device
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

USBD_HandleTypeDef hUsbDeviceFS;
USBD_CDC_HandleTypeDef *hcdcdc;

void MidiSender(std::deque<Note> &deqNote, uint8_t *buf) {
	if (hcdcdc->TxState == 0) { //(0==свободно, !0==занято)
		int s = 0;
		int i = deqNote.size() * 8;
		if (i > 512) {
			deqNote.resize(64);
			i = 512;
		}
		for (auto &n : deqNote) {
			buf[s] = 9; // ??
			buf[s + 1] = 176; // 176 (for hi-res midi)
			buf[s + 2] = 88; // 88 (for hi-res midi)
			buf[s + 3] = n.lo; // velocity xx.75

			buf[s + 4] = 9; // ?
			buf[s + 5] = uint8_t(n.mO); // 0x90(144) - note on, 0x80(128) - note off
			buf[s + 6] = n.note; // number note
			buf[s + 7] = n.hi; // velocity 86.xx

			s += 8;
		}

		deqNote.clear();

		USBD_CDC_SetTxBuffer(&hUsbDeviceFS, buf, i);
		USBD_CDC_TransmitPacket(&hUsbDeviceFS);
	}
}

void MX_USB_DEVICE_Init(void) {

	if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK) {
		Error_Handler();
	}
	if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_CDC) != USBD_OK) {
		Error_Handler();
	}
	if (USBD_CDC_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS)
			!= USBD_OK) {
		Error_Handler();
	}
	if (USBD_Start(&hUsbDeviceFS) != USBD_OK) {
		Error_Handler();
	}

	HAL_PWREx_EnableUSBVoltageDetector();

	uint8_t txbuf[8]; //start midi mini test
//on
	txbuf[0] = 9; //??
	txbuf[1] = 144; //0x90(144) - note on, 0x80(128) - note off //0xB0 - Control Change (for hi-res midi) | channel
	txbuf[2] = 55; //note	// 88 (for hi-res midi)
	txbuf[3] = 60; //vel	// velocity xx.75 (for hi-res midi)

//off
	txbuf[4] = 9; //0x09	//??
	txbuf[5] = 128; //0x90(144) - note on, 0x80(128) - note off | channel
	txbuf[6] = 55; //note
	txbuf[7] = 120; //vel	//velocity 86.xx (for hi-res midi)

	HAL_Delay(600);

	CDC_Transmit_FS(txbuf, 8);
	HAL_Delay(100);
	CDC_Transmit_FS(txbuf, 8);

	hcdcdc = (USBD_CDC_HandleTypeDef*) hUsbDeviceFS.pClassData; // свободно для отправки? "if (hcdcdc->TxState == ...)"; (0==свободно, !0==занято)
}

extern "C" {
}
