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


DaqHatInstrument::DaqHatInstrument(uint8_t address) : _address(address) {
}

DaqHatInstrument::~DaqHatInstrument() {
}

int DaqHatInstrument::openNoinit() {
	return 0;
}

int DaqHatInstrument::open() {
	return 0;
}

int DaqHatInstrument::close() {
	return 0;
}

int DaqHatInstrument::setIEPE(uint8_t channel, bool enable) {
	_enabled[channel] = enable;
	return 0;
}

int DaqHatInstrument::getIEPE(uint8_t channel, bool* enabled) {
	*enabled = _enabled[channel];
	return 0;
}

int DaqHatInstrument::setSensitivity(uint8_t channel, double sensitivity) {
	_sensitivity[channel] = sensitivity;
	return 0;
}

int DaqHatInstrument::getSensitivity(uint8_t channel, double* sensitivity) {
	*sensitivity = _sensitivity[channel];
	return 0;
}

int DaqHatInstrument::setClock(uint8_t source, double sampleRate) {
	_source = source;
	_sampleRate = sampleRate;
	return 0;
}

int DaqHatInstrument::getClock(uint8_t* source, double* sampleRate, bool* synced) {
	*sampleRate = _sampleRate;
	*synced = 1;
	*source = _source;
	return 0;
}

int DaqHatInstrument::startScan(uint8_t channel_mask, uint32_t samples_per_channel, uint32_t options){
	_channel_mask = channel_mask;
	_samples_per_channel = samples_per_channel;
	_options = options;

	// to do allocate and fill buffer
	return 0;
}

int DaqHatInstrument::readScan(uint16_t * status, int32_t samples_per_channel, double timeout, double * buffer, uint32_t buffer_size_samples, uint32_t * samples_read_per_channel) {
	return 0;
}

int DaqHatInstrument::scanChannelCount() {
	return _channel_mask == 3 ? 2 : 1;
}

int DaqHatInstrument::stopScan() {
	return 0;
}

int DaqHatInstrument::cleanupScan() {
	if (_buffer) {
		free(_buffer);
		_buffer = nullptr;
	}
	return 0;
}

int DaqHatInstrument::enableTrigger() {
	// todo
	return -10;
}

int DaqHatInstrument::disableTrigger() {
	// todo
	return -10;
}

