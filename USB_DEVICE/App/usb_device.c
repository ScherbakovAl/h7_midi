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

void MidiSendOn(uint8_t vel_midi_hi, uint8_t vel_midi_lo, uint8_t note) {
	uint8_t txbuf[8]; // можно размер и 512
	txbuf[0] = 9; 	// ??
	txbuf[1] = 176; // 176 (for hi-res midi)
	txbuf[2] = 88; 	// 88 (for hi-res midi)
	txbuf[3] = vel_midi_lo; // velocity xx.75

	txbuf[4] = 9; 	// ?
	txbuf[5] = 144; // 0x90(144) - note on, 0x80(128) - note off
	txbuf[6] = note; // number note
	txbuf[7] = vel_midi_hi; //velocity 86.xx
	USBD_CDC_EP0_RxReady();
	if (hcdcdc->TxState == 0) { //(0==свободно, !0==занято)
		USBD_CDC_SetTxBuffer(&hUsbDeviceFS, txbuf, 8);
		USBD_CDC_TransmitPacket(&hUsbDeviceFS);//долгая функция, что можно упростить?
	}
}

void MidiSendOff(uint8_t vel_midi_hi, uint8_t vel_midi_lo, uint8_t note) {
	uint8_t txbuf[8];
	txbuf[0] = 9; //??
	txbuf[1] = 176; // 176 (for hi-res midi)
	txbuf[2] = 88; // 88 (for hi-res midi)
	txbuf[3] = vel_midi_lo; //velocity xx.75

	txbuf[4] = 9; //??
	txbuf[5] = 128; // 0x90(144) - note on, 0x80(128) - note off
	txbuf[6] = note; //number note
	txbuf[7] = vel_midi_hi; //velocity 86.xx

	CDC_Transmit_FS(txbuf, 8);
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
	txbuf[0] = 9; 	//??
	txbuf[1] = 144; //0x90(144) - note on, 0x80(128) - note off //0xB0 - Control Change (for hi-res midi) | channel
	txbuf[2] = 55;	//note	// 88 (for hi-res midi)
	txbuf[3] = 60; 	//vel	// velocity xx.75 (for hi-res midi)

//off
	txbuf[4] = 9;	//0x09	//??
	txbuf[5] = 128;	//0x90(144) - note on, 0x80(128) - note off | channel
	txbuf[6] = 55;	//note
	txbuf[7] = 120; //vel	//velocity 86.xx (for hi-res midi)

	HAL_Delay(600);

	CDC_Transmit_FS(txbuf, 8);
	HAL_Delay(100);
	CDC_Transmit_FS(txbuf, 8);

	hcdcdc = (USBD_CDC_HandleTypeDef*) hUsbDeviceFS.pClassData; // свободно для отправки? "if (hcdcdc->TxState == ...)"; (0==свободно, !0==занято)
}
