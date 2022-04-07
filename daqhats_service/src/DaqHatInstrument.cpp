/*
 * DaqHatInstrument.cpp
 *
 *  Created on: 06 apr. 2022
 *      Author: jancu
 */

#include "DaqHatInstrument.h"

#include <stdio.h>
#include <unistd.h>
#include <iostream>

#include <daqhats/daqhats.h>

DaqHatInstrument::DaqHatInstrument(uint8_t address) : _address(address) {
}

DaqHatInstrument::~DaqHatInstrument() {
}

int DaqHatInstrument::openNoinit() {
	int result = RESULT_SUCCESS;
//	select_hat_device(HAT_ID_MCC_172, &_address);
	result = mcc172_open(_address);
	return result;
}

int DaqHatInstrument::open() {
	int result = DaqHatInstrument::openNoinit();
	// todo init functionality

	return result;
}

int DaqHatInstrument::close() {
	int result = mcc172_close(_address);
	return result;
}

int DaqHatInstrument::setIEPE(uint8_t channel, bool enable) {
	int result = mcc172_iepe_config_write(_address, channel, enable ? 1 : 0);
	return result;
}

int DaqHatInstrument::getIEPE(uint8_t channel, bool* enabled) {
	uint8_t config = 0U;
	int result = mcc172_iepe_config_read(_address, channel, &config);
	*enabled = (config == 1);
	return result;
}

int DaqHatInstrument::enableTrigger() {
	// todo
	return -1;
}

int DaqHatInstrument::disableTrigger() {
	// todo
	return -1;
}

