/*
 * DaqHatInstrument.h
 *
 *  Created on: 06 apr. 2022
 *      Author: jancu
 */

// C++ port of:
/**
 * @file  pifacedigital.h
 * @brief A simple static library for controlling Daq Hat.
 * See mcp23s17.h for more register definitions.
 *
 * Copyright (C) 2013 Thomas Preston <thomas.preston@openlx.org.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DAQHATINSTRUMENT_H_
#define DAQHATINSTRUMENT_H_

#include <cstdint>


using namespace std;

class DaqHatInstrument {
public:

  // * @param address : the address set with the hat jumper
  DaqHatInstrument( uint8_t address );
  virtual ~DaqHatInstrument();

  /**
   * Opens and initialises a Daq Hat with the specified hardware
   * address.
   *
   * Example:
   *
   *     dh->open();
   *
   * @return int 0 on success
   */
  virtual int open();

  /**
   * Opens a Daq Hat with the specified hardware address  without
   * initialising it.
   *
   * Example:
   *
   *     dh->openNoinit();
   *
   * @return int 0 on success
   */
  virtual int openNoinit();

  /**
   * Closes a Daq Hat with the specified hardware address.
   *
   * Example:
   *
   *     dh->close();
   *
   * @return int 0 on success
   */
  virtual int close();

  /**
   * Enables or disables the IEPE power supply for a channel.
   * @param channel
   * @param enable
   * @return int 0 on success
   */
  virtual int setIEPE( uint8_t channel, bool enable);

  /**
   * Queries the IEPE power supply for a channel.
   * @param channel
   * @param enabled
   * @return int 0 on success
   */
  virtual int getIEPE(uint8_t channel, bool* enabled);

  /**
     * Configures the sensitivity value for a channel.
     * @param channel
     * @param sensitivity
     * @return int 0 on success
     */
    virtual int setSensitivity(uint8_t channel, double sensitivity);

  /**
   * Queries the sensitivity value for a channel.
   * @param channel
   * @param sensitivity
   * @return int 0 on success
   */
  virtual int getSensitivity(uint8_t channel, double* sensitivity);

  /**
     * Configures the clock value for a channel.
     * @param source
     * @param sampleRate
     * @return int 0 on success
     */
    virtual int setClock(uint8_t source, double sampleRate);

  /**
   * Queries the clock value for a channel.
   * @param source
   * @param sampleRate
   * @param synced
   * @return int 0 on success
   */
  virtual int getClock(uint8_t* source, double* sampleRate, bool* synced);


  virtual int startScan(uint8_t channel_mask, uint32_t samples_per_channel,
		  uint32_t options);

  virtual int readScan(uint16_t * status, int32_t samples_per_channel,
		  double timeout, double * buffer, uint32_t buffer_size_samples,
		  uint32_t * samples_read_per_channel);

  virtual int scanChannelCount();

  virtual int stopScan();

  virtual int cleanupScan();

  /**
   * Enables trigger.
   *
   * Example:
   *
   *    dh->enableTrigger();
   *
   * @return int 0 on success
   */
  virtual int enableTrigger(void);

  /**
   * Disables trigger.
   *
   * Example:
   *
   *    dh->disableTrigger();
   *
   * @return int 0 on success
   */
  virtual int disableTrigger(void);


protected:
  uint8_t _address;

// dummy
  uint8_t _channel;
  bool _enabled[2] = {false,false};
  double _sensitivity[2] = {1000.0, 1000.0};
  uint8_t _source = 1;
  double _sampleRate = 64.0;
  uint8_t _channel_mask = 1;
  uint32_t samples_per_channel = 0U;
  uint32_t options = 0;

  double* _buffer = nullptr;


private:

};

#endif /* DAQHATINSTRUMENT_H_ */
