/*
 * maain.h
 *
 *  Created on: Sep 23, 2022
 *      Author: scherbakov
 */

#ifndef INC_MAAIN_H_
#define INC_MAAIN_H_


//#pragma once

#include <queue>
#include <bitset>

using namespace std;

#include "main.h"
//#include "usbd_midi_if.h"

class gpio_bsrr {
public:
	void sh_ld();	// +A0	(+SH/LD)
	void _sh_ld();	// -A0	(-SH/LD)
	void clk();		// +A1	(+CLK)
	void _clk();	// -A1	(-CLK)
	void andd();	// +A2	(AND)
	void _andd();	// -A2	(AND)
	void andd_off();	// +A2	(AND)
	void _andd_off();	// -A2	(AND)

//	void distort_A0();	// test_A0
//	void distort_A1();	// test_A1
//	void distort_A2();	// test_A2
//
//	void distort_E8();	// test_E8
//	void distort_E10();	// test_E10
//	void distort_E12();	// test_E12
//	void distort_E14();	// test_E14
private:
	const unsigned int sh_ld_bsrr = 0x0010; // оторвался роводок, переставил на А4 pin
	const unsigned int _sh_ld_bsrr = 0x00100000; // оторвался роводок, переставил на А4 pin
	const unsigned int clk_bsrr = 0x2;
	const unsigned int _clk_bsrr = 0x20000;
	const unsigned int andd_bsrr = 0x0040; // оторвался роводок, переставил на А6 pin
	const unsigned int _andd_bsrr = 0x00400000; // оторвался роводок, переставил на А6 pin
	const unsigned int and_off = 0x8;
	const unsigned int _and_off = 0x80000;
//	const unsigned int test_E8_bsrr_on = 0x100;
//	const unsigned int test_E8_bsrr_off = 0x1000000;
//	const unsigned int test_E10_bsrr_on = 0x400;
//	const unsigned int test_E10_bsrr_off = 0x4000000;
//	const unsigned int test_E12_bsrr_on = 0x1000;
//	const unsigned int test_E12_bsrr_off = 0x10000000;
//	const unsigned int test_E14_bsrr_on = 0x4000;
//	const unsigned int test_E14_bsrr_off = 0x40000000;

};

class Numbers {
public:
	Numbers(const unsigned int &channel, const unsigned int &mux_);
	unsigned int number;
	unsigned int mu;
	unsigned int cha;
};

class Counter {
public:
	void reset();
	void twenty_times();
	void event_occurred();
	bool check();
private:
	unsigned int times = 0;
	unsigned int event = 0;
};

class Keys {//explicit ??????????????????????????????????????????
public:
	void number_note_setter();
	void init_bit_mask();
	void wheel();
	void mask_load_to_imr(const unsigned int &a);
	void check();
	void interrupt(const unsigned int &channel);
	void update_bit_channel(const unsigned int &channel);
	void timer_save(const Numbers &nu, const unsigned int &m);
	void send_midi(const unsigned int &nu, const unsigned int &Ti);

private:
	static const unsigned int keyss = 176;
	static const unsigned int channel_bits = 11;
	static const unsigned int size_mux = 16;

	static const unsigned int max_midi = 127;
	static const unsigned int overmax_midi = 125;

	static const unsigned int time_min = 200;
	static const unsigned int time_max = 100'000;
	static const unsigned int re_triggering = 50'000;
	static const unsigned int time_to_clean_up = 50'000;
	static const unsigned int divisible = 8'600'000;

	queue<Numbers> quee;
	bitset<channel_bits> bits[size_mux];
	gpio_bsrr gpio;
	Counter counter;
	volatile unsigned int mux = 0;
	unsigned int timer[keyss] = { };
	unsigned int sensor_flag[keyss] = { };
	uint8_t notes[keyss];
	static const unsigned int nule = 0;
	static const unsigned int one = 1;
	static const unsigned int two = 2;

	//	unsigned int test_in_speed = 0; //for tests
	//	unsigned int test_in_time = 0; //for tests
};



#endif /* INC_MAAIN_H_ */
