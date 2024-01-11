/*
 * midi_keyboard.cpp
 *
 *  Created on: Sep 23, 2022
 *  edited 11.02.2024
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

void numberS::set(cuint &channel, cuint &m) {
	number = (channel << 4) + m;
	mux = m;
	cha = channel;
}

void Keys::numberNoteSetter() {
	uint zz = 21;
	for (uint i = 0; i < sensors;) {
		notes[i] = zz;
		notes[i + 1] = zz;
		++zz;
		i += 2;
	}
}

void Keys::initBitMask() {
	numberNoteSetter();
	for (uint s = 0; s < sizeMux; ++s) {
		bitsMidiOn[s].set();
		bitsMidiOff[s].reset();
	}
	HAL_Delay(10);
}

void Keys::wheel() {
	gpio.AndOffHi_Off();		// чтобы не грелись микрухи управляющие "off" ??
	initBitMask();
	mux.setSizeMux(sizeM);
	SysTick->CTRL = 0;

	while (1) {
		// gpio.Test1(); //синхронизация осциллографа // в данном проекте не включено gpio должным образом!!!

		midiOnOrOff = NowOnOrOff::midiOn;
		for (uint i = 0; i < 100; ++i) {
			maskLoadMidiOn();
			gpio.ShLdHi_On();	// =
			gpio.AndLo_On();	// =
			gpio.ShLdLo_On();	// |=
			gpio.AndHi_On();	// |=

			for (uint i = one; i < sizeMux; ++i) {
				maskLoadMidiOn();
				gpio.ClkLo_On();	// =
				gpio.AndLo_On();	// =
				gpio.ClkHi_On();	// |=
				gpio.AndHi_On();	// |=
			}
			check();
		}
		midiOnOrOff = NowOnOrOff::midiOff;
		maskLoadMidiOff();
		gpio.ShLdHi_Off();		// =
		gpio.AndOffLo_Off();	// =
		gpio.ShLdLo_Off();		// |=
		gpio.AndOffHi_Off();	// |=

		for (uint i = one; i < sizeMux; ++i) {
			maskLoadMidiOff();
			gpio.ClkLo_Off();	// =
			gpio.AndOffLo_Off();	// =
			gpio.ClkHi_Off();	// |=
			gpio.AndOffHi_Off();	// |=
		}
	}
}

void Keys::maskLoadMidiOn() {
	mux.toggle();
	EXTI->IMR1 = bitsMidiOn[mux.get()].to_ulong();
}

void Keys::maskLoadMidiOff() {
	mux.toggle();
	EXTI->IMR1 = bitsMidiOff[mux.get()].to_ulong();
}

void Keys::check() {
	if (!queeOn.empty()) {
		auto &front = queeOn.front();
		if (TIM2->CNT - timer[front.number] > timeToCleanUp) {
			bitsMidiOn[front.mux].set(front.cha);
			queeOn.pop_front();
		}
	}
}

void Keys::interrupt(cuint &channel) {
//	gpio.Test1();// test // в данном проекте не включено gpio должным образом!!!
	numberS nnumb;
	nnumb.set(channel, mux.get());
	if (midiOnOrOff == NowOnOrOff::midiOn) {
		bitsMidiOn[nnumb.mux].reset(channel);
		queeOn.push_back(nnumb);
		timerSave(nnumb);
	} else {
		MidiSendOff(120, 13, notes[nnumb.number]);
		bitsMidiOff[nnumb.mux].reset(channel);
	}
//	gpio.Test2();// test // в данном проекте не включено gpio должным образом!!!
}

void Keys::timerSave(const numberS &nu) {
	auto Now = TIM2->CNT;
	if (nu.mux % 2 == 0) {
		timer[nu.number] = Now;
	} else {
		auto time = Now - timer[nu.number - 1];
		timer[nu.number] = Now;
		sendMidi(nu.number, time);
		bitsMidiOff[nu.mux - 1].set(nu.cha);
	}
}

void Keys::sendMidi(cuint &nu, cuint &t) {
	auto midi_speed = divisible / t;	// 200-50000
	auto midi_hi = midi_speed / maxMidi;
	auto midi_lo = midi_speed - midi_hi * maxMidi;
	MidiSendOn(midi_hi, midi_lo, notes[nu]);
}

uint muxer::get() const {
	return mux;
}

void muxer::toggle() {
	if (mux < size) {
		++mux;
	} else {
		mux = 0;
	}
}

void muxer::setSizeMux(cuint &s) {
	size = s;
}

