/*
 * midi_keyboard.h
 *
 *  Created on: Sep 23, 2022
 *      Author: scherbakov
 */

#pragma once

#include <queue>
#include <bitset>
//#include "main.h"
//#include "usbd_midi_if.h"

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

	void Test1();
	void Test2();

private:
	const unsigned int shLdHi = 0x1;
	const unsigned int shLdLo = 0x10000;
	const unsigned int clkHi = 0x2;
	const unsigned int clkLo = 0x20000;
	const unsigned int andOnHi = 0x4;
	const unsigned int andOnLo = 0x40000;
	const unsigned int andOffHi = 0x8;
	const unsigned int andOffLo = 0x80000;
	const unsigned int test1On = 0x010;
	const unsigned int test1Off = 0x0100000;
	const unsigned int test2On = 0x020;
	const unsigned int test2Off = 0x0200000;
};

class numberS {
public:
	void set(const unsigned int &channel, const unsigned int &mux_);
	unsigned int number = 0;
	unsigned int mu = 0;
	unsigned int cha = 0;
};

class counterR {
public:
	void reset();
	void twentyTimes();
	void eventOccurred();
	bool check();
private:
	unsigned int times = 0;
	unsigned int event = 0;
};

class Keys {	//explicit ?
public:
	void numberNoteSetter();
	void initBitMask();
	void wheel();
	void maskLoadMidiOn(const unsigned int &a);
	void maskLoadMidiOff(const unsigned int &a);
	void check();
	void interrupt(const unsigned int &channel);
	void updateBitChannel(const unsigned int &channel);
	void timerSave(const numberS &nu, const unsigned int &m);
	void sendMidi(const unsigned int &nu, const unsigned int &Ti);

private:
	static const unsigned int sensors = 176;
	static const unsigned int channelBits = 11;
	static const unsigned int sizeMux = 16;

	static const unsigned int maxMidi = 127;
	static const unsigned int overmaxMidi = 125;

	static const unsigned int timeMin = 200;		//static?
	static const unsigned int timeMax = 100'000;
	static const unsigned int reTriggering = 50'000;
	static const unsigned int timeToCleanUp = 50'000;
	static const unsigned int divisible = 9'000'000;

	NowOnOrOff midiOnOrOff = NowOnOrOff::midiOn;
	std::queue<numberS> queeOn;
//	std::queue<numberS> queeOff;
	std::bitset<channelBits> bitsMidiOn[sizeMux];
	std::bitset<channelBits> bitsMidiOff[sizeMux];	// << size "channelBits"??
	gpioBsrr gpio;
	counterR counter;
	volatile unsigned int mux = 0;
	unsigned int timer[sensors] = { };
	unsigned int sensorFlag[sensors] = { };
	uint8_t notes[sensors];
	static const unsigned int zero = 0;
	static const unsigned int one = 1;
	static const unsigned int two = 2;

	//tests
	int test1 = 0;
	int test2 = 0;
};
