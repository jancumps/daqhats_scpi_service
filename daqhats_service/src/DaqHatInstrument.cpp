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

int DaqHatInstrument::setSensitivity(uint8_t channel, double sensitivity) {
	int result = mcc172_a_in_sensitivity_write(_address, channel, sensitivity);
	return result;
}

int DaqHatInstrument::getSensitivity(uint8_t channel, double* sensitivity) {
	int result = mcc172_a_in_sensitivity_read(_address, channel, sensitivity);
	return result;
}

int DaqHatInstrument::setClock(uint8_t source, double sampleRate) {
	int result = mcc172_a_in_clock_config_write(_address, source, sampleRate);
	return result;
}

int DaqHatInstrument::getClock(uint8_t* source, double* sampleRate, bool* synced) {
	uint8_t sync;
	int result = mcc172_a_in_clock_config_read(_address,
			source, sampleRate, &sync);
	*synced = (sync==0) ? false : true;
	return result;
}

int DaqHatInstrument::startScan(uint8_t channel_mask, uint32_t samples_per_channel, uint32_t options){
	int result = mcc172_a_in_scan_start(_address, channel_mask, samples_per_channel, options);
	return result;
}

int DaqHatInstrument::readScan(uint16_t * status, int32_t samples_per_channel, double timeout, double * buffer, uint32_t buffer_size_samples, uint32_t * samples_read_per_channel) {
	int result = mcc172_a_in_scan_read(_address, status, samples_per_channel, timeout, buffer, buffer_size_samples, samples_read_per_channel);
	return result;
}

int DaqHatInstrument::scanChannelCount() {
	int result = mcc172_a_in_scan_channel_count(_address);
	return result;
}

int DaqHatInstrument::stopScan() {
	int result = mcc172_a_in_scan_stop(_address);
	return result;
}

int DaqHatInstrument::cleanupScan() {
	int result = mcc172_a_in_scan_cleanup(_address);
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

