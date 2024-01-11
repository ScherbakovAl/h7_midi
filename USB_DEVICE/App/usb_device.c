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

void MidiSendOn(uint8_t vel_midi_hi, uint8_t vel_midi_lo, uint8_t note) {
	uint8_t txbuf[8];
	txbuf[0] = 0x9; //??
	txbuf[1] = 0xB0 | 0x00; //0xB0 - Control Change (for hi-res midi) | channel
	txbuf[2] = 0x7F & 88; // 88 (for hi-res midi)
	txbuf[3] = 0x7F & vel_midi_lo; //velocity xx.75

	txbuf[4] = 0x9; //??
	txbuf[5] = 0x90 | 0x00; //0x8 - note off, 0x9 - note on | channel
	txbuf[6] = 0x7F & note; //number note
	txbuf[7] = 0x7F & vel_midi_hi; //velocity 86.xx

//		USBD_LL_Transmit(&hUsbDeviceFS, 0x81, txbuf, 8);
	CDC_Transmit_FS(txbuf, 8);	// usbd_cdc_if.c //?
}
void MidiSendOff(uint8_t vel_midi_hi, uint8_t vel_midi_lo, uint8_t note) {
	uint8_t txbuf[8];
	txbuf[0] = 0x9; //??
	txbuf[1] = 0xB0 | 0x00; //0xB0 - Control Change (for hi-res midi) | channel
	txbuf[2] = 0x7F & 88; // 88 (for hi-res midi)
	txbuf[3] = 0x7F & vel_midi_lo; //velocity xx.75

	txbuf[4] = 0x9; //??
	txbuf[5] = 0x80 | 0x00; //0x8 - note off, 0x9 - note on | channel
	txbuf[6] = 0x7F & note; //number note
	txbuf[7] = 0x7F & vel_midi_hi; //velocity 86.xx

//		USBD_LL_Transmit(&hUsbDeviceFS, 0x81, txbuf, 8);
	CDC_Transmit_FS(txbuf, 8);	// usbd_cdc_if.c //?
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

	uint8_t txbuf[8];
	txbuf[0] = 0x9; //??
	txbuf[1] = 0x90; //0xB0 - Control Change (for hi-res midi) | channel
	txbuf[2] = 52; //note // 88 (for hi-res midi)
	txbuf[3] = 70; //vel  // velocity xx.75

	txbuf[4] = 0x9; //??
	txbuf[5] = 0x80 | 0x00; //0x80 - note off, 0x90 - note on | channel
	txbuf[6] = 0x7F & 52; //number note
	txbuf[7] = 0x7F & 86; //velocity 86.xx

//	HAL_Delay(100);

	for (int a = 0; a <= 4; ++a) {//куда пропадает первое сообщение?
//		USBD_LL_Transmit(&hUsbDeviceFS, 0x81, txbuf, 8);
		CDC_Transmit_FS(txbuf, 8);	// usbd_cdc_if.c
		HAL_Delay(100);
	}

}


