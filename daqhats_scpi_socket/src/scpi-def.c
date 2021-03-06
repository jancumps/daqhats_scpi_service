/*-
 * Copyright (c) 2012-2013 Jan Breuer,
 *
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file   scpi-def.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 *
 * @brief  SCPI parser test
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scpi/scpi.h"
#include "scpi-def.h"

#include "linux_scpi_socket.h"

char instrument_payload[22]; // two last to represent \n\0



static scpi_result_t SCPI_DaqIepe(scpi_t * context) {

  scpi_bool_t param1;
  int32_t numbers[1];

  // retrieve the channel. Can be 0 - 1
  SCPI_CommandNumbers(context, numbers, 1, 0);
  if (! ((numbers[0] > -1) && (numbers[0] < 2) )) {
    SCPI_ErrorPush(context, SCPI_ERROR_INVALID_SUFFIX);
    return SCPI_RES_ERR;
  }

  /* read first parameter if present */
  if (!SCPI_ParamBool(context, &param1, TRUE)) {
    return SCPI_RES_ERR;
  }


  memset(instrument_payload, '.', sizeof(instrument_payload));
  instrument_payload[0] = 'w';
  instrument_payload[1] = 'i';
  instrument_payload[2] = 'e';
  instrument_payload[3] = 'p';
  instrument_payload[4] = 'e';
  (instrument_payload[5] = '0' + numbers[0]); // only works if the 0 - 7 characters in the character set are consecutive. todo: Sue me.
  instrument_payload[6] = param1 ? '1' : '0';

  sendToInstrument(instrument_payload, sizeof(instrument_payload));

  return SCPI_RES_OK;
}


static scpi_result_t SCPI_DaqIepeQ(scpi_t * context) {
	int32_t numbers[1];

	// retrieve the output. Can be 0 - 1
	SCPI_CommandNumbers(context, numbers, 1, 0);
	if (! ((numbers[0] > -1) && (numbers[0] < 2) )) {
		SCPI_ErrorPush(context, SCPI_ERROR_INVALID_SUFFIX);
		return SCPI_RES_ERR;
	}

	memset(instrument_payload, '.', sizeof(instrument_payload));
	instrument_payload[0] = 'r';
	instrument_payload[1] = 'i';
	instrument_payload[2] = 'e';
	instrument_payload[3] = 'p';
	instrument_payload[4] = 'e';
	(instrument_payload[5] = '0' + numbers[0]); // only works if the 0 - 7 characters in the character set are consecutive. todo: Sue me.

	sendToInstrument(instrument_payload, sizeof(instrument_payload));


	// parse the reply
	SCPI_ResultBool(context, ((instrument_payload[6] - '0') > 0) );  // only works if the 0 - 1 characters in the character set are consecutive. todo: Sue me.
	return SCPI_RES_OK;
}

static scpi_result_t SCPI_DaqSensitivity(scpi_t * context) {

  double param1;
  int32_t numbers[1];

  // retrieve the channel. Can be 0 - 1
  SCPI_CommandNumbers(context, numbers, 1, 0);
  if (! ((numbers[0] > -1) && (numbers[0] < 2) )) {
    SCPI_ErrorPush(context, SCPI_ERROR_INVALID_SUFFIX);
    return SCPI_RES_ERR;
  }

  /* read first parameter if present */
  if (!SCPI_ParamDouble(context, &param1, TRUE)) {
    return SCPI_RES_ERR;
  }


  memset(instrument_payload, '.', sizeof(instrument_payload));
  instrument_payload[0] = 'w';
  instrument_payload[1] = 's';
  instrument_payload[2] = 'e';
  instrument_payload[3] = 'n';
  instrument_payload[4] = 's';
  (instrument_payload[5] = '0' + numbers[0]); // only works if the 0 - 7 characters in the character set are consecutive. todo: Sue me.
  sprintf(&instrument_payload[6], "%lf", param1);

  sendToInstrument(instrument_payload, sizeof(instrument_payload));

  return SCPI_RES_OK;
}


static scpi_result_t SCPI_DaqSensitivityQ(scpi_t * context) {
	int32_t numbers[1];

	// retrieve the output. Can be 0 - 1
	SCPI_CommandNumbers(context, numbers, 1, 0);
	if (! ((numbers[0] > -1) && (numbers[0] < 2) )) {
		SCPI_ErrorPush(context, SCPI_ERROR_INVALID_SUFFIX);
		return SCPI_RES_ERR;
	}

	memset(instrument_payload, '.', sizeof(instrument_payload));
	instrument_payload[0] = 'r';
	instrument_payload[1] = 's';
	instrument_payload[2] = 'e';
	instrument_payload[3] = 'n';
	instrument_payload[4] = 's';
	(instrument_payload[5] = '0' + numbers[0]); // only works if the 0 - 7 characters in the character set are consecutive. todo: Sue me.

	sendToInstrument(instrument_payload, sizeof(instrument_payload));

	// todo this will return more than one character !!!!
	// parse the reply
	double value = strtod(&instrument_payload[6], NULL);
	SCPI_ResultFloat(context, value);
	return SCPI_RES_OK;
}

static scpi_result_t SCPI_DaqClock(scpi_t * context) {
  uint32_t param1; // source
  double param2; // sample rate

  /* read first parameter if present */
  if (!SCPI_ParamUInt32(context, &param1, TRUE)) {
    return SCPI_RES_ERR;
  }

  /* read second parameter if present */
  if (!SCPI_ParamDouble(context, &param2, TRUE)) {
    return SCPI_RES_ERR;
  }

  memset(instrument_payload, '.', sizeof(instrument_payload));
  instrument_payload[0] = 'w';
  instrument_payload[1] = 'c';
  instrument_payload[2] = 'l';
  instrument_payload[3] = 'c';
  instrument_payload[4] = 'k';
  (instrument_payload[5] = '0' + param1); // only works if the 0 - 7 characters in the character set are consecutive. todo: Sue me.
  sprintf(&instrument_payload[6], "%lf", param2);

  sendToInstrument(instrument_payload, sizeof(instrument_payload));

  return SCPI_RES_OK;
}


static scpi_result_t SCPI_DaqClockSourceQ(scpi_t * context) {
	memset(instrument_payload, '.', sizeof(instrument_payload));
	instrument_payload[0] = 'r';
	instrument_payload[1] = 'c';
	instrument_payload[2] = 'l';
	instrument_payload[3] = 's';
	instrument_payload[4] = 'o';

	sendToInstrument(instrument_payload, sizeof(instrument_payload));

	// parse the reply
	uint32_t value = instrument_payload[5] - '0';
	SCPI_ResultUInt32(context, value);
	return SCPI_RES_OK;
}

static scpi_result_t SCPI_DaqClockSampleRateQ(scpi_t * context) {
	memset(instrument_payload, '.', sizeof(instrument_payload));
	instrument_payload[0] = 'r';
	instrument_payload[1] = 'c';
	instrument_payload[2] = 'l';
	instrument_payload[3] = 's';
	instrument_payload[4] = 'a';

	sendToInstrument(instrument_payload, sizeof(instrument_payload));

	// parse the reply
	double value = strtod(&instrument_payload[5], NULL);
	SCPI_ResultFloat(context, value);
	return SCPI_RES_OK;
}

static scpi_result_t SCPI_DaqClockSyncQ(scpi_t * context) {
	memset(instrument_payload, '.', sizeof(instrument_payload));
	instrument_payload[0] = 'r';
	instrument_payload[1] = 'c';
	instrument_payload[2] = 'l';
	instrument_payload[3] = 's';
	instrument_payload[4] = 'y';

	sendToInstrument(instrument_payload, sizeof(instrument_payload));

	// parse the reply
	SCPI_ResultBool(context, ((instrument_payload[5] - '0') > 0) );  // only works if the 0 - 1 characters in the character set are consecutive. todo: Sue me.
	return SCPI_RES_OK;
}

static scpi_result_t SCPI_DaqScanStart(scpi_t * context) {

	  uint32_t param1; // channel mask
	  uint32_t param2; // buffer size
	  uint32_t param3; // options

	  /* read first parameter if present */
	  if (!SCPI_ParamUInt32(context, &param1, TRUE)) {
	    return SCPI_RES_ERR;
	  }

	  /* read second parameter if present */
	  if (!SCPI_ParamUInt32(context, &param2, TRUE)) {
	    return SCPI_RES_ERR;
	  }

	  /* read third parameter if present */
	  if (!SCPI_ParamUInt32(context, &param3, TRUE)) {
	    return SCPI_RES_ERR;
	  }


  memset(instrument_payload, '.', sizeof(instrument_payload));
  instrument_payload[0] = 'w';
  instrument_payload[1] = 's';
  instrument_payload[2] = 'c';
  instrument_payload[3] = 's';
  instrument_payload[4] = 't';
  (instrument_payload[5] = '0' + param1); // only works if the 0 - 7 characters in the character set are consecutive. todo: Sue me.
  sprintf(&instrument_payload[6], "%02X", param3); // in hex, and guaranteed 2 positions
  sprintf(&instrument_payload[8], "%d", param2);


  sendToInstrument(instrument_payload, sizeof(instrument_payload));

  return SCPI_RES_OK;
}

static scpi_result_t SCPI_DaqScanStop(scpi_t * context) {

  memset(instrument_payload, '.', sizeof(instrument_payload));
  instrument_payload[0] = 'w';
  instrument_payload[1] = 's';
  instrument_payload[2] = 'c';
  instrument_payload[3] = 's';
  instrument_payload[4] = 'p';

  sendToInstrument(instrument_payload, sizeof(instrument_payload));

  return SCPI_RES_OK;
}

static scpi_result_t SCPI_DaqScanCleanup(scpi_t * context) {

  memset(instrument_payload, '.', sizeof(instrument_payload));
  instrument_payload[0] = 'w';
  instrument_payload[1] = 's';
  instrument_payload[2] = 'c';
  instrument_payload[3] = 'c';
  instrument_payload[4] = 'l';

  sendToInstrument(instrument_payload, sizeof(instrument_payload));

  return SCPI_RES_OK;
}

/**
 * Reimplement IEEE488.2 *TST?
 *
 * Result should be 0 if everything is ok
 * Result should be 1 if something goes wrong
 *
 * Return SCPI_RES_OK
 */
static scpi_result_t My_CoreTstQ(scpi_t * context) {

    SCPI_ResultInt32(context, 0);

    return SCPI_RES_OK;
}

const scpi_command_t scpi_commands[] = {
    /* IEEE Mandated Commands (SCPI std V1999.0 4.1.1) */
    { .pattern = "*CLS", .callback = SCPI_CoreCls,},
    { .pattern = "*ESE", .callback = SCPI_CoreEse,},
    { .pattern = "*ESE?", .callback = SCPI_CoreEseQ,},
    { .pattern = "*ESR?", .callback = SCPI_CoreEsrQ,},
    { .pattern = "*IDN?", .callback = SCPI_CoreIdnQ,},
    { .pattern = "*OPC", .callback = SCPI_CoreOpc,},
    { .pattern = "*OPC?", .callback = SCPI_CoreOpcQ,},
    { .pattern = "*RST", .callback = SCPI_CoreRst,},
    { .pattern = "*SRE", .callback = SCPI_CoreSre,},
    { .pattern = "*SRE?", .callback = SCPI_CoreSreQ,},
    { .pattern = "*STB?", .callback = SCPI_CoreStbQ,},
    { .pattern = "*TST?", .callback = My_CoreTstQ,},
    { .pattern = "*WAI", .callback = SCPI_CoreWai,},

    /* Required SCPI commands (SCPI std V1999.0 4.2.1) */
    {.pattern = "SYSTem:ERRor[:NEXT]?", .callback = SCPI_SystemErrorNextQ,},
    {.pattern = "SYSTem:ERRor:COUNt?", .callback = SCPI_SystemErrorCountQ,},
    {.pattern = "SYSTem:VERSion?", .callback = SCPI_SystemVersionQ,},



    {.pattern = "STATus:QUEStionable[:EVENt]?", .callback = SCPI_StatusQuestionableEventQ,},
    /* {.pattern = "STATus:QUEStionable:CONDition?", .callback = scpi_stub_callback,}, */
    {.pattern = "STATus:QUEStionable:ENABle", .callback = SCPI_StatusQuestionableEnable,},
    {.pattern = "STATus:QUEStionable:ENABle?", .callback = SCPI_StatusQuestionableEnableQ,},

    {.pattern = "STATus:PRESet", .callback = SCPI_StatusPreset,},

    /* DAQ hat */
    {.pattern = "DAQ:IEPe#?", .callback = SCPI_DaqIepeQ,},
    {.pattern = "DAQ:IEPe#", .callback = SCPI_DaqIepe,},
	{.pattern = "DAQ:SENSitivity#?", .callback = SCPI_DaqSensitivityQ,},
	{.pattern = "DAQ:SENSitivity#", .callback = SCPI_DaqSensitivity,},
	{.pattern = "DAQ:CLOCk:SOURce?", .callback = SCPI_DaqClockSourceQ,},
	{.pattern = "DAQ:CLOCk:SAMPlerate?", .callback = SCPI_DaqClockSampleRateQ,},
	{.pattern = "DAQ:CLOCk:SYNCed?", .callback = SCPI_DaqClockSyncQ,},
	{.pattern = "DAQ:CLOCk", .callback = SCPI_DaqClock,},
	{.pattern = "DAQ:SCAN:STArt", .callback = SCPI_DaqScanStart,},
	{.pattern = "DAQ:SCAN:STOp", .callback = SCPI_DaqScanStop,},
	{.pattern = "DAQ:SCAN:CLEanup", .callback = SCPI_DaqScanCleanup,},

    {.pattern = "SYSTem:COMMunication:TCPIP:CONTROL?", .callback = SCPI_SystemCommTcpipControlQ,},

    SCPI_CMD_LIST_END
};

scpi_interface_t scpi_interface = {
    .error = SCPI_Error,
    .write = SCPI_Write,
    .control = SCPI_Control,
    .flush = SCPI_Flush,
    .reset = SCPI_Reset,
};

scpi_bool_t scpi_instrument_input(const char * data, int len) {
    return SCPI_Input(&scpi_context, data, len);
}


char scpi_input_buffer[SCPI_INPUT_BUFFER_LENGTH];
scpi_error_t scpi_error_queue_data[SCPI_ERROR_QUEUE_SIZE];

scpi_t scpi_context;


