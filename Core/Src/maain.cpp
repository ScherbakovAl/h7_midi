/*
 * maain-2.cpp
 *
 *  Created on: Sep 23, 2022
 *      Author: scherbakov
 */

#include "main.h"
#include "maain.h"
//#include "usbd_midi.h"
//test github
using namespace std;

void gpio_bsrr::sh_ld() {
	GPIOA->BSRR = sh_ld_bsrr;// =
}
void gpio_bsrr::_sh_ld() {
	GPIOA->BSRR |= _sh_ld_bsrr;// |=
}
void gpio_bsrr::clk() {
	GPIOA->BSRR |= clk_bsrr;// |=
}
void gpio_bsrr::_clk() {
	GPIOA->BSRR = _clk_bsrr;// =
}
void gpio_bsrr::andd() {
	GPIOA->BSRR |= andd_bsrr;// |=
}
void gpio_bsrr::_andd() {
	GPIOA->BSRR = _andd_bsrr;// = (последнее изменение было сдесь (по эксперименту с " |= и ="))
}

void gpio_bsrr::distort_A0() {
	GPIOA->BSRR |= sh_ld_bsrr;
	GPIOA->BSRR |= _sh_ld_bsrr;
}

void gpio_bsrr::distort_A1() {
	GPIOA->BSRR |= clk_bsrr;
	GPIOA->BSRR |= _clk_bsrr;
}

void gpio_bsrr::distort_A2() {
	GPIOA->BSRR |= andd_bsrr;
	GPIOA->BSRR |= _andd_bsrr;
}

void gpio_bsrr::distort_E8() {
	GPIOE->BSRR |= test_E8_bsrr_on;
	GPIOE->BSRR |= test_E8_bsrr_off;
}
void gpio_bsrr::distort_E10() {
	GPIOE->BSRR |= test_E10_bsrr_on;
	GPIOE->BSRR |= test_E10_bsrr_off;
}
void gpio_bsrr::distort_E12() {
	GPIOE->BSRR |= test_E12_bsrr_on;
	GPIOE->BSRR |= test_E12_bsrr_off;
}
void gpio_bsrr::distort_E14() {
	GPIOE->BSRR |= test_E14_bsrr_on;
	GPIOE->BSRR |= test_E14_bsrr_off;
}

Numbers::Numbers(const unsigned int &channel, const unsigned int &mux_) :
		number((channel << 4) + mux_), mu(mux_), cha(channel) {
}

void Counter::reset() {
	times = 0;
	event = 0;
}
void Counter::twenty_times() {
	++times;
}
void Counter::event_occurred() {
	++event;
}
bool Counter::check() {
	if (event != 0 && times > 20) {
		return (true);
	} else {
		return (false);
	}
	return (false);
}

void Keys::number_note_setter() {
	uint8_t zz = 69;
	for (int i = 0; i < keyss;) {
		notes[i] = zz;
		notes[i + 1] = zz;
		++zz;
		i += 2;
	}
}

void Keys::init_bit_mask() {
	number_note_setter();
	for (int sat = nule; sat < size_mux; ++sat) {
		//		bits[sat].set(0);//..
		//		bits[sat].set(1);//..
		//		bits[sat].set(2);//..
		//		bits[sat].set(3);//.. 1-3  не припаян?
		bits[sat].set(4);		//.. 1-2
		bits[sat].set(5);		//.. 1-1
		bits[sat].set(6);		//.. 2-1
		bits[sat].set(7);		//.. 2-2
		bits[sat].set(8);		//.. 2-3
		//		bits[sat].set(9);//..
		//		bits[sat].set(10);//..
	}
}

void Keys::wheel() {
	while (1) {
		mask_load_to_imr(0);
		gpio.sh_ld();//=
		gpio._andd();//=
		gpio._sh_ld();//|=
		gpio.andd();//|=

		for (int i = one; i < size_mux; ++i) {
			mask_load_to_imr(i);

			//			gpio.distort_E8();//for tests

			gpio._clk();//=
			gpio._andd();//=
			gpio.clk();//|=
			gpio.andd();//|=
		}
		check();
		counter.twenty_times();
		if (counter.check()) {
			//			USBD_MIDI_SendPacket();
			counter.reset();
			//			gpio.distort_E8();
		}
	}
}

void Keys::mask_load_to_imr(const unsigned int &a) {
	mux = a;
	EXTI->IMR1 = bits[a].to_ulong();
}

void Keys::check() {
	if (!quee.empty()) {
		auto &front = quee.front(); //Возвращает ссылку на первый элемент в начале queue
		if (TIM2->CNT - timer[front.number] > time_to_clean_up) {
			sensor_flag[front.number] = nule;
			bits[front.mu].set(front.cha); //Устанавливает бит в позиции pos в значение value.
			quee.pop(); //Удаляет элемент из начала queue
		}
	}
}

void Keys::interrupt(const unsigned int &channel) {

	//	gpio.distort_E10();//for tests

	unsigned int muu = mux;
	bits[muu].reset(channel);
	Numbers nnumb(channel, muu);
	quee.push(nnumb); //Добавляет элемент в конец queue
	timer_save(nnumb, muu);

	//	gpio.distort_E10();//for tests
	//	gpio.distort_E12();//for tests
}

void Keys::timer_save(const Numbers &nu, const unsigned int &m) {
	auto Ti = TIM2->CNT;
	auto &numb = nu.number;
	auto next = numb + one;
	if (m % two) {
		if (sensor_flag[numb] == nule
				&& Ti - timer[numb - one] > re_triggering) { // какие здесь данные должны быть?
			timer[numb] = Ti;
			sensor_flag[numb] = one;
		}
	} else if (auto time = Ti - timer[next]; sensor_flag[next] == one
			&& time > time_min && time < time_max) {
		timer[numb] = Ti;
		send_midi(numb, time);
		sensor_flag[next] = two;
	}
}

void Keys::send_midi(const unsigned int &nu, const unsigned int &Ti) {
	auto midi_speed = divisible / Ti; //200-50000

	auto midi_hi = midi_speed / max_midi;
	auto midi_lo = midi_speed - midi_hi * max_midi; //			midi_speed - (midi_speed / max_midi * max_midi);

	//	test_in_time = superTimer;
	//	test_in_speed = Ti;

	//	void USBD_AddNoteOn(uint8_t cable, uint8_t ch, uint8_t note, uint8_t vel);
	//	USBD_AddNoteOn2(notes[nu],
	//			midi_speed < max_midi ? midi_speed : overmax_midi);
	//	USBD_AddNoteOn3(notes[nu], midi_lo, midi_hi);
	//	USBD_SendMidiMessages();
	//	USBD_MIDI_SendPacket2();

	counter.event_occurred();
}

