/*
 * midi_keyboard.h
 *
 *  Created on: Sep 23, 2022
 *      Author: scherbakov
 */

#pragma once

#include "usb_device.h"
#include <deque>
#include <bitset>

using uint = unsigned int;
using cuint = const uint;

enum class NowOnOrOff {
	midiOn, midiOff
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

	void Test1();		// в данном проекте не включено gpio должным образом!!!
	void Test2();

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
};

class numberS {
public:
	void set(cuint &channel, cuint &m);
	uint number = 0;
	uint mux = 0;
	uint cha = 0;
};

class muxer {
public:
	void toggle();
	uint get() const;
	void setSizeMux(cuint &s);
private:
	uint mux = 0;		//volatile??? надо протестить!
	uint size;
};

class Keys {	//explicit ?
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
	void sendMidi(cuint &nu, cuint &t);

	static cuint sensors = 176;
	static cuint channelBits = 11;
	static cuint sizeMux = 16;
	cuint maxMidi = 127;//static?
	cuint reTriggering = 50'000;//static?
	cuint timeToCleanUp = 50'000;//static?
	cuint divisible = 9'000'000;//static?
	cuint sizeM = sizeMux;//static?

	muxer mux;
	gpioBsrr gpio;
	NowOnOrOff midiOnOrOff = NowOnOrOff::midiOn;
	std::deque<numberS> queeOn;
	std::bitset<channelBits> bitsMidiOn[sizeMux];
	std::bitset<channelBits> bitsMidiOff[sizeMux];
	uint timer[sensors] = { };
	uint notes[sensors];
	static cuint zero = 0;
	static cuint one = 1;
	static cuint two = 2;
};
