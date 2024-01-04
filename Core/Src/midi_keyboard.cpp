/*
 * midi_keyboard.cpp
 *
 *  Created on: Sep 23, 2022
 *      Author: scherbakov
 */

#include <midi_keyboard.h>

// ################################################## for ON
void gpioBsrr::ShLdHi_On() {
	GPIOA->BSRR |= shLdHi;		// =
}

void gpioBsrr::ShLdLo_On() {
	GPIOA->BSRR |= shLdLo;		// |=
}

void gpioBsrr::ClkHi_On() {
	GPIOA->BSRR |= clkHi;		// |=
}

void gpioBsrr::ClkLo_On() {
	GPIOA->BSRR |= clkLo;		// =
}

void gpioBsrr::AndHi_On() {
	GPIOA->BSRR |= andOnHi;		// |=
}

void gpioBsrr::AndLo_On() {
	GPIOA->BSRR |= andOnLo;		// =
}

// ################################################## for OFF
void gpioBsrr::ShLdHi_Off() {
	GPIOA->BSRR |= shLdHi;		// =
}

void gpioBsrr::ShLdLo_Off() {
	GPIOA->BSRR |= shLdLo;		// |=
}

void gpioBsrr::ClkHi_Off() {
	GPIOA->BSRR |= clkHi;		// |=
}

void gpioBsrr::ClkLo_Off() {
	GPIOA->BSRR |= clkLo;		// =
}

void gpioBsrr::AndOffHi_Off() {
	GPIOA->BSRR |= andOffHi;	// |=
}

void gpioBsrr::AndOffLo_Off() {
	GPIOA->BSRR |= andOffLo;	// =
}

// ################################################## for tests
// в данном проекте не включено gpio должным образом!!!
void gpioBsrr::Test1() {
	GPIOA->BSRR |= test1On;		// |=
	GPIOA->BSRR |= test1Off;	// =
}

void gpioBsrr::Test2() {
	GPIOA->BSRR |= test2On;		// |=
	GPIOA->BSRR |= test2Off;	// =
}
// в данном проекте не включено gpio должным образом!!!
// ##################################################

void numberS::set(const unsigned int &channel, volatile unsigned int &mux_) {
	number = (channel << 4) + mux_;
	mu = mux_;
	cha = channel;
}

void Keys::numberNoteSetter() {
	uint8_t zz = 21;
	for (unsigned int i = 0; i < sensors;) {
		notes[i] = zz;
		notes[i + 1] = zz;
		++zz;
		i += 2;
	}
}

void Keys::initBitMask() {
	numberNoteSetter();
	for (unsigned int s = 0; s < sizeMux; ++s) {
		bitsMidiOn[s].set();
		bitsMidiOff[s].reset();
	}
	HAL_Delay(10);
}

void Keys::wheel() {
	gpio.AndOffHi_Off();		// чтобы не грелись микрухи управляющие "off" ??
	initBitMask();
	SysTick->CTRL = 0;

	while (1) {
		// gpio.Test1(); //синхронизация осциллографа // в данном проекте не включено gpio должным образом!!!

		midiOnOrOff = NowOnOrOff::midiOn;
		for (int i = 0; i < 100; ++i) {
			maskLoadMidiOn(0);
			gpio.ShLdHi_On();	// =
			gpio.AndLo_On();	// =
			gpio.ShLdLo_On();	// |=
			gpio.AndHi_On();	// |=

			for (unsigned int i = one; i < sizeMux; ++i) {
				maskLoadMidiOn(i);
				gpio.ClkLo_On();	// =
				gpio.AndLo_On();	// =
				gpio.ClkHi_On();	// |=
				gpio.AndHi_On();	// |=
			}
			check();
		}
		midiOnOrOff = NowOnOrOff::midiOff;
		maskLoadMidiOff(0);
		gpio.ShLdHi_Off();		// =
		gpio.AndOffLo_Off();	// =
		gpio.ShLdLo_Off();		// |=
		gpio.AndOffHi_Off();	// |=

		for (unsigned int i = one; i < sizeMux; ++i) {
			test1 = i;
			maskLoadMidiOff(i);
			gpio.ClkLo_Off();	// =
			gpio.AndOffLo_Off();	// =
			gpio.ClkHi_Off();	// |=
			gpio.AndOffHi_Off();	// |=
		}
	}
}

void Keys::maskLoadMidiOn(const unsigned int &a) {
	mux = a;
	EXTI->IMR1 = bitsMidiOn[a].to_ulong();
}

void Keys::maskLoadMidiOff(const unsigned int &a) {
	mux = a;
	EXTI->IMR1 = bitsMidiOff[a].to_ulong();
}

void Keys::check() {
	if (!queeOn.empty()) {
		auto &front = queeOn.front();
		if (TIM2->CNT - timer[front.number] > timeToCleanUp) {
			bitsMidiOn[front.mu].set(front.cha);
			queeOn.pop();
		}
	}
}

void Keys::interrupt(const unsigned int &channel) {
	gpio.Test1();// test // в данном проекте не включено gpio должным образом!!!
	numberS nnumb;
	nnumb.set(channel, mux);
	if (midiOnOrOff == NowOnOrOff::midiOn) {
		bitsMidiOn[nnumb.mu].reset(channel);
		queeOn.push(nnumb);
		timerSave(nnumb);
	} else {
		MidiSendOff(120, 13, notes[nnumb.number]);
		bitsMidiOff[nnumb.mu].reset(channel);
	}
	gpio.Test2();// test // в данном проекте не включено gpio должным образом!!!
}

void Keys::timerSave(const numberS &nu) {
	auto Now = TIM2->CNT;
	if (nu.mu % 2 == 0) {
		timer[nu.number] = Now;
	} else {
		auto time = Now - timer[nu.number - 1];
		timer[nu.number] = Now;
		sendMidi(nu.number - 1, time);
		bitsMidiOff[nu.mu - 1].set(nu.cha);
	}
}

void Keys::sendMidi(const unsigned int &nu, const unsigned int &Ti) {
	auto midi_speed = divisible / Ti;	// 200-50000
	auto midi_hi = midi_speed / maxMidi;
	auto midi_lo = midi_speed - midi_hi * maxMidi;
	MidiSendOn(midi_hi, midi_lo, notes[nu]);
}
