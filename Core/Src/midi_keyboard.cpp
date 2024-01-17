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
// ################################################## for OFF

void gpioBsrr::Test1() {
//	if(TIM2->CNT - testTimer > us){
//		 testTimer = TIM2->CNT;
	GPIOA->BSRR |= test1On;		// |=
	GPIOA->BSRR |= test1Off;	// =
//	}
}

void gpioBsrr::Test2() {
	GPIOA->BSRR |= test2On;		// |=
	GPIOA->BSRR |= test2Off;	// =
}

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
		midiOnOrOff = NowOnOrOff::midiOn;
//		for (uint i = 0; i < 100; ++i) {
		maskLoadMidiOn();
		gpio.ShLdHi_On();	// =
		gpio.AndLo_On();	// =
		gpio.ShLdLo_On();	// |=
		gpio.AndHi_On();	// |=
		mux.toggle();

		for (uint o = one; o < sizeMux; ++o) {
			maskLoadMidiOn();
			gpio.ClkLo_On();	// =
			gpio.AndLo_On();	// =
			gpio.ClkHi_On();	// |=
			gpio.AndHi_On();	// |=
			mux.toggle();
		}
		check();
//		}
		midiOnOrOff = NowOnOrOff::midiOff;
		maskLoadMidiOff();
		gpio.ShLdHi_Off();		// =
		gpio.AndOffLo_Off();	// =
		gpio.ShLdLo_Off();		// |=
		gpio.AndOffHi_Off();	// |=
		mux.toggle();

		for (uint p = one; p < sizeMux; ++p) {
			maskLoadMidiOff();
			gpio.ClkLo_Off();	// =
			if (!(p % 2))
				gpio.AndOffLo_Off();	// =
			gpio.ClkHi_Off();	// |=
			if (!(p % 2))
				gpio.AndOffHi_Off();	// |=
			mux.toggle();
		}
	}
}

void Keys::maskLoadMidiOn() {
	EXTI->IMR1 = bitsMidiOn[mux.get()].to_ulong();
}

void Keys::maskLoadMidiOff() {
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
	if (!midiChannelFree.empty() && sendFree == isMidiSendReady::READY) {
		sendFree = isMidiSendReady::NOREADY;
		auto &front = midiChannelFree.front();
		MidiSendOn(front.hi, front.lo, front.note);
		midiChannelFree.pop_front();
	}
}

void Keys::interrupt(cuint &channel) {
	numberS nnumb;
	nnumb.set(channel, mux.get());
	if (midiOnOrOff == NowOnOrOff::midiOn) {
		bitsMidiOn[nnumb.mux].reset(channel);
		queeOn.push_back(nnumb);
		timerSave(nnumb);
	} else {	//добавлять в очередь и отправлять спустя время..
		MidiSendOff(120, 13, notes[nnumb.number]);
		bitsMidiOff[nnumb.mux].reset(channel);
	}
}

void Keys::timerSave(const numberS &nu) {
	auto Now = TIM2->CNT;
	if (nu.mux % 2 == 0) {
		timer[nu.number] = Now;
	} else {
		auto time = Now - timer[nu.number - 1];
		timer[nu.number] = Now;

		// в очередь. Если off не сработал - то сразу добавить событие off, иначе просто миди on
		sendMidi(nu.number, time); //<<<<<<<<<<<<<<<<<<<<<<
//		sendMidi(nu.number + 2, time + 200); //<<<<<<<<<<<<<<<<<<<<<<
//		sendMidi(nu.number + 4, time + 400); //<<<<<<<<<<<<<<<<<<<<<<
//		sendMidi(nu.number + 6, time + 600); //<<<<<<<<<<<<<<<<<<<<<<

		bitsMidiOff[nu.mux - 1].set(nu.cha);
	}
}

void Keys::sendMidi(cuint &nu, cuint &t) {
	auto midi_speed = divisible / t;	// 200-50000
	auto midi_hi = midi_speed / maxMidi;
	auto midi_lo = midi_speed - midi_hi * maxMidi;

	//пока костыли для прверок
	midiChannelFree.push_back( { midi_hi, midi_lo, notes[nu] });

}

uint muxer::get() const {
	return mux;
}

void muxer::toggle() {
	if (mux < size - 1) {
		++mux;
	} else {
		mux = 0;
	}
}

void muxer::setSizeMux(cuint &s) {
	size = s;
}

void Keys::midiFree() {
	sendFree = isMidiSendReady::READY;
}
