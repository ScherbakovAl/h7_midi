/*
 * midi_keyboard.cpp
 *
 *  Created on: Sep 23, 2022
 *      Author: scherbakov
 */
// git test in "изменение_из_midi_controller............"
#include <midi_keyboard.h>
#include "usb_device.h"
//#include "main.h"
//#include "usbd_midi.h"

//using namespace std;

//  ################################################## for ON
void gpioBsrr::ShLdHi_On() {
	GPIOA->BSRR |= shLdHi; 	// =
}
void gpioBsrr::ShLdLo_On() {
	GPIOA->BSRR |= shLdLo; 	// |=
}
void gpioBsrr::ClkHi_On() {
	GPIOA->BSRR |= clkHi;		// |=
}
void gpioBsrr::ClkLo_On() {
	GPIOA->BSRR |= clkLo;		// =
}
void gpioBsrr::AndHi_On() {
	GPIOA->BSRR |= andOnHi;	// |=
}
void gpioBsrr::AndLo_On() {
	GPIOA->BSRR |= andOnLo; 	// =
}
//  ################################################## for OFF
void gpioBsrr::ShLdHi_Off() {
	GPIOA->BSRR |= shLdHi; 	// =
}
void gpioBsrr::ShLdLo_Off() {
	GPIOA->BSRR |= shLdLo; 	// |=
}
void gpioBsrr::ClkHi_Off() {
	GPIOA->BSRR |= clkHi;		// |=
}
void gpioBsrr::ClkLo_Off() {
	GPIOA->BSRR |= clkLo;		// =
}
void gpioBsrr::AndOffHi_Off() {
	GPIOA->BSRR |= andOffHi;		// |=
}
void gpioBsrr::AndOffLo_Off() {
	GPIOA->BSRR |= andOffLo;		// =
}
//  ################################################## for tests
void gpioBsrr::Test1() {
	GPIOA->BSRR |= test1On;		// |=
	GPIOA->BSRR |= test1Off;		// =
}
void gpioBsrr::Test2() {
	GPIOA->BSRR |= test2On;		// |=
	GPIOA->BSRR |= test2Off;		// =
}
//  ##################################################

void numberS::set(const unsigned int &channel, const unsigned int &mux_) {
	number = (channel << 4) + mux_;
	mu = mux_;
	cha = channel;
}

void counterR::reset() {
	times = 0;
	event = 0;
}
void counterR::twentyTimes() {
	++times;
}
void counterR::eventOccurred() {
	++event;
}
bool counterR::check() {
	if (event != 0 && times > 20) {
		return (true);
	} else {
		return (false);
	}
	return (false);
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
	for (unsigned int sat = zero; sat < sizeMux; ++sat) {
		bitsMidiOn[sat].set(0);		// =1
		bitsMidiOn[sat].set(1);		// =2
		bitsMidiOn[sat].set(2);		// =3
		bitsMidiOn[sat].set(3);		// =4
		bitsMidiOn[sat].set(4);		// =5
		bitsMidiOn[sat].set(5);		// =6
		bitsMidiOn[sat].set(6);		// =7
		bitsMidiOn[sat].set(7);		// =8
		bitsMidiOn[sat].set(8);		// =9
		bitsMidiOn[sat].set(9);		// =10
		bitsMidiOn[sat].set(10);	// =11
	}
	for (unsigned int sat = zero; sat < sizeMux; ++sat) {
		bitsMidiOff[sat].reset(0);		// =1
		bitsMidiOff[sat].reset(1);		// =2
		bitsMidiOff[sat].reset(2);		// =3
		bitsMidiOff[sat].reset(3);		// =4
		bitsMidiOff[sat].reset(4);		// =5
		bitsMidiOff[sat].reset(5);		// =6
		bitsMidiOff[sat].reset(6);		// =7
		bitsMidiOff[sat].reset(7);		// =8
		bitsMidiOff[sat].reset(8);		// =9
		bitsMidiOff[sat].reset(9);		// =10
		bitsMidiOff[sat].reset(10);		// =11
	}
	HAL_Delay(800);
}
void Keys::wheel() {
	gpio.AndOffHi_Off();		// чтобы не грелись микрухи управляющие "off"
	initBitMask();
	SysTick->CTRL = 0;// работает нормально с этой настройкой? (без него HAL_Delay отваливается)

	while (1) {
//		gpio.Test1();		//синхронизация осциллографа
#define ON
#ifdef ON
		midiOnOrOff = NowOnOrOff::midiOn;
		for (int i = 0; i < 100; ++i) {
			maskLoadMidiOn(0); 	// почему-то в такой последовательности работает
			gpio.ShLdHi_On(); 		//  =
			gpio.AndLo_On();		//  =
			gpio.ShLdLo_On(); 		// |=
			gpio.AndHi_On();		// |=

			for (unsigned int i = one; i < sizeMux; ++i) {
				maskLoadMidiOn(i);// почему-то в такой последовательности работает
				gpio.ClkLo_On();		//  =
				gpio.AndLo_On();		//  =
				gpio.ClkHi_On();		// |=
				gpio.AndHi_On();		// |=
			}
			check();
		}
#endif
		midiOnOrOff = NowOnOrOff::midiOff;
		maskLoadMidiOff(0);	// почему-то в такой последовательности работает
		gpio.ShLdHi_Off(); 		//  =
		gpio.AndOffLo_Off();		//  =
		gpio.ShLdLo_Off(); 		// |=
		gpio.AndOffHi_Off();		// |=

		for (unsigned int i = one; i < sizeMux; ++i) {
			test1 = i;
			maskLoadMidiOff(i);	// почему-то в такой последовательности работает
			gpio.ClkLo_Off();		//  =
			gpio.AndOffLo_Off();		//  =
			gpio.ClkHi_Off();		// |=
			gpio.AndOffHi_Off();		// |=
		}

//		check();

//		counter.twenty_times();
//		if (counter.check()) {
////						USBD_MIDI_SendPacket();
//			counter.reset();
//		}

//		test_timer = TIM2->CNT; // for test
//		if (test_timer - test_timer_older > 1'000'000) {
//			test_timer_older = test_timer;
//			MidiSend(68, 35, 55);
//		}
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
//	if (midiOnOrOff == NowOnOrOff::midiOn) {
	if (!queeOn.empty()) {
		auto &front = queeOn.front(); //Возвращает ссылку на первый элемент в начале queue
		if (TIM2->CNT - timer[front.number] > timeToCleanUp) {
			sensorFlag[front.number] = zero;
			bitsMidiOn[front.mu].set(front.cha); //Устанавливает бит в позиции pos в значение value.
			queeOn.pop(); //Удаляет элемент из начала queue
		}
	}
//	} else {
//		// реализовать Off
//	}
}
void Keys::interrupt(const unsigned int &channel) {
	gpio.Test1(); // <<<<<<<<<<<< !!!!!!!!!!!!!! работает с оптимизацией -О1 (с -О3 не работает)
	unsigned int muu = mux;
	numberS nnumb;
	nnumb.set(channel, muu);
	if (midiOnOrOff == NowOnOrOff::midiOn) {
		bitsMidiOn[muu].reset(channel);
		queeOn.push(nnumb); //Добавляет элемент в конец queue
//		if(queeOn.size() > 50)//когда оптимизация -О3 сваливается сюда
//		{
//			queeOn.pop();
//		}
		timerSave(nnumb, muu);
	} else {
//		if (muu % 2 == 1) { //?? надо ли?
		bitsMidiOff[muu].reset(channel);
//		MidiSendOff(120, 13, notes[nnumb.number]);
//		}
	}
	gpio.Test2();
}
void Keys::timerSave(const numberS &nu, const unsigned int &m) {
	auto Ti = TIM2->CNT;
	auto &numb = nu.number;
	auto prev = numb - one;

	if (m % 2 == 0) {			// - прохождение первого сенсора
		if (sensorFlag[numb] == zero && Ti - timer[numb] > reTriggering) { // какие здесь данные должны быть?
			timer[numb] = Ti;
			sensorFlag[numb] = one;
		}

	} else if (auto time = Ti - timer[prev]; sensorFlag[prev] == one // - прохождение второго сенсора
	&& time > timeMin && time < timeMax) {
		timer[numb] = Ti;
		sendMidi(prev, time);
		sensorFlag[numb] = two;
		bitsMidiOff[m - 1].set(nu.cha);
	}
}
void Keys::sendMidi(const unsigned int &nu, const unsigned int &Ti) {
	auto midi_speed = divisible / Ti; //200-50000

	auto midi_hi = midi_speed / maxMidi;
	auto midi_lo = midi_speed - midi_hi * maxMidi; //			midi_speed - (midi_speed / max_midi * max_midi);

//	MidiSend(midi_hi, midi_lo, notes[nu]);//86, 75, 58//midi_hi, midi_lo, notes[nu]

	//	test_in_time = superTimer;
	//	test_in_speed = Ti;

	//	void USBD_AddNoteOn(uint8_t cable, uint8_t ch, uint8_t note, uint8_t vel);
	//	USBD_AddNoteOn2(notes[nu],
	//			midi_speed < max_midi ? midi_speed : overmax_midi);
	//	USBD_AddNoteOn3(notes[nu], midi_lo, midi_hi);
	//	USBD_SendMidiMessages();
	//	USBD_MIDI_SendPacket2();
//	counter.event_occurred();
}

