/*
 *  midi_keyboard.h
 *
 *  Created on: Sep 23, 2022
 *     Author: scherbakov.al
 */

#pragma once

#include "usb_device.h"
#include <deque>
#include <bitset>

using uint = unsigned int;
using cuint = const uint;

enum class OnOrOff : uint8_t {
	midiOff = 128, midiOn = 144
};

class gpioBsrr {
public:
	void ShLdHi_On();		// +A0	(+SH/LD)
	void ShLdLo_On();		// -A0	(-SH/LD)
	void ClkHi_On();		// +A1	(+CLK)
	void ClkLo_On();		// -A1	(-CLK)
	void AndHi_On();		// +A2	(AND_ON)
	void AndLo_On();		// -A2	(AND_ON)

	void ShLdHi_Off();
	void ShLdLo_Off();
	void ClkHi_Off();
	void ClkLo_Off();
	void AndOffHi_Off();		// +A3	(AND_OFF)
	void AndOffLo_Off();		// -A3	(AND_OFF)

	void Test1();
	void Test2();
	void Test3();
	void Test4();

private:
	cuint shLdHi = 0x1;
	cuint shLdLo = 0x10000;
	cuint clkHi = 0x2;
	cuint clkLo = 0x20000;
	cuint andOnHi = 0x4;
	cuint andOnLo = 0x40000;
	cuint andOffHi = 0x8;
	cuint andOffLo = 0x80000;
	cuint test1On = 0x010;
	cuint test1Off = 0x0100000;
	cuint test2On = 0x020;
	cuint test2Off = 0x0200000;
	cuint test3On = 0x040;
	cuint test3Off = 0x0400000;
	cuint test4On = 0x080;
	cuint test4Off = 0x0800000;
};

class numberS {
public:
	void set(cuint &channel, cuint &m);
	uint number = 0;
	uint mux = 0;
	uint cha = 0;
};

class Note {
public:
	cuint hi = 0;
	cuint lo = 0;
	cuint note = 0;
	OnOrOff mO = OnOrOff::midiOn;
};

class muxer {
public:
	void toggle();
	uint get() const;
	void setSizeMux(cuint &s);
private:
	uint mux = 0;
	uint size;
};

class Keys {
public:
	void wheel();
	void interrupt(cuint &channel);
private:
	void numberNoteSetter();
	void initBitMask();
	void maskLoadMidiOn();
	void maskLoadMidiOff();
	void check();
	void timerSave(const numberS &nu);
	void sendMidi(cuint &nu, cuint &t, OnOrOff &mO);

	static cuint sensors = 176;
	static cuint channelBits = 11;
	static cuint sizeMux = 16;
	cuint maxMidi = 127;
	cuint divisible = 7'900'000;
	cuint reTriggering = uint(float(divisible) / 1.1f / 127.0f);
	cuint timeToCleanUp = reTriggering;
	cuint max = divisible / (maxMidi * maxMidi);
	cuint off_lo = uint(float(divisible) / 1.0f / 127.0f);
	cuint off_hi = uint(float(divisible) / 60.0f / 127.0f);
	cuint sizeM = sizeMux;

	muxer mux;
	gpioBsrr gpio;
	OnOrOff midiOnOrOff = OnOrOff::midiOn;
	std::deque<numberS> dequeOn;
	std::deque<Note> dequeNotes;
	std::deque<unsigned long int> led;
	std::bitset<channelBits> bitsMidiOn[sizeMux];
	std::bitset<channelBits> bitsMidiOff[sizeMux];
	uint timer[sensors] = { };
	uint notes[sensors];
	uint8_t bufNotes[512] = { };
	static cuint zero = 0;
	static cuint one = 1;
	static cuint two = 2;
};
