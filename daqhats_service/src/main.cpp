/*
 * main.cpp
 *
 *  Created on: 06 apr. 2022
 *      Author: jancu
 */

#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <boost/asio.hpp>


#include <stdlib.h>

#include "DaqHatInstrument.h"
#include <daqhats/daqhats.h>

using boost::asio::ip::tcp;



uint8_t getChannel(std::string* payload) {
	return std::stoi(payload->substr(5, 1));
}

std::string formatDouble(double d) {
	std::ostringstream streamObj;
	// Set Fixed -Point Notation
	streamObj << std::fixed;
	//Add double to stream
	streamObj << d;
	// Get string from output string stream
	return streamObj.str();
}

void riepe(DaqHatInstrument &dh, std::string* x);
void wiepe(DaqHatInstrument &dh, std::string* x);
void rsens(DaqHatInstrument &dh, std::string* x);
void wsens(DaqHatInstrument &dh, std::string* x);
void rclso(DaqHatInstrument &dh, std::string* x);
void rclsa(DaqHatInstrument &dh, std::string* x);
void rclsy(DaqHatInstrument &dh, std::string* x);
void wclck(DaqHatInstrument &dh, std::string* x);
void wscst(DaqHatInstrument &dh, std::string* x, tcp::acceptor* dataacceptor, tcp::socket* datasocket, thread** scanThread);
void wscsp(DaqHatInstrument &dh, std::string* x);
void wsccl(DaqHatInstrument &dh, std::string* x);

void scan(DaqHatInstrument* dh, tcp::acceptor* dataacceptor, tcp::socket* datasocket,uint32_t samples_per_channel);

int main(int argc, char** argv) {
	int commandportno = 0;
	int dataportno = 0;
	thread* scanThread = nullptr;

	try {
		if (argc != 4) {
			std::cerr << "Usage: command_port data_port daqaddress" << std::endl;
			return 1;
		}

		commandportno = atoi(argv[1]);
		dataportno = atoi(argv[2]);
		boost::asio::io_service io_service;

		tcp::endpoint commandendpoint(tcp::v4(), (commandportno));
		tcp::acceptor commandacceptor(io_service, commandendpoint);

		tcp::acceptor dataacceptor(io_service, tcp::endpoint(tcp::v4(), dataportno ));
		tcp::socket datasocket(io_service);

		DaqHatInstrument dh(atoi(argv[3]));
		dh.open();

		for (;;) {
			std::string x;
			tcp::iostream commandstream;
			commandacceptor.accept(*commandstream.rdbuf());
			while (commandstream >> x) {
				// process the commands
				if (x.compare(0, 5, "riepe") == 0) { // IEPE
     	           riepe(dh, &x);
				} else if (x.compare(0, 5, "wiepe") == 0){
					wiepe(dh, &x);
				} else if (x.compare(0, 5, "rsens") == 0) { // sensitivity
					rsens(dh, &x);
				} else if (x.compare(0, 5, "wsens") == 0){
					wsens(dh, &x);
				} else if (x.compare(0, 5, "rclso") == 0) { // clock
					rclso(dh, &x);
				} else if (x.compare(0, 5, "rclsa") == 0) {
					rclsa(dh, &x);
				} else if (x.compare(0, 5, "rclsy") == 0) {
					rclsy(dh, &x);
				} else if (x.compare(0, 5, "wclck") == 0){
					wclck(dh, &x);
				} else if (x.compare(0, 5, "wscst") == 0){ // scan start (submits a stream thread)
					wscst(dh, &x, &dataacceptor, &datasocket, &scanThread);
				} else if (x.compare(0, 5, "wscsp") == 0){ // scan stop
					wscsp(dh, &x);
				} else if (x.compare(0, 5, "wsccl") == 0){ // scan clean
					wsccl(dh, &x);
				    datasocket.close();
					delete scanThread;
				}

				// send reply
				commandstream << x; // the endl is passed on from the source. << std::endl;
			}
		}

		dh.close();
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}
	return 0;
}

void riepe(DaqHatInstrument &dh, std::string* x) {
	bool enabled = false;
	dh.getIEPE(getChannel(x), &enabled);
     x->replace(6,1, enabled ? "1" : "0");
}

void wiepe(DaqHatInstrument &dh, std::string* x) {
	uint8_t enable = std::stoi(x->substr(6, 1));
	dh.setIEPE(getChannel(x), enable);
}

void rsens(DaqHatInstrument &dh, std::string* x) {
	double sens = 0.0;
	dh.getSensitivity(getChannel(x), &sens);
	std::string d = formatDouble(sens);
	x->replace(6, d.length(), d);
}

void wsens(DaqHatInstrument &dh, std::string* x) {
	double sensitivity = std::stod(x->substr(6));
	dh.setSensitivity(getChannel(x), sensitivity);
}

void rclso(DaqHatInstrument &dh, std::string* x) {
	uint8_t source = 0;
	double sampleRate = 0.0;
	bool synced = false;
	dh.getClock(&source, &sampleRate, &synced);
	x->replace(5, 1, std::to_string(source));
}

void rclsa(DaqHatInstrument &dh, std::string* x) {
	uint8_t source = 0;
	double sampleRate = 0.0;
	bool synced = false;
	dh.getClock(&source, &sampleRate, &synced);
	std::string d = formatDouble(sampleRate);
	x->replace(5, d.length(), d);
}

void rclsy(DaqHatInstrument &dh, std::string* x) {
	uint8_t source = 0;
	double sampleRate = 0.0;
	bool synced = false;
	dh.getClock(&source, &sampleRate, &synced);
	x->replace(5, 1, synced ? "1" : "0");
}

void wclck(DaqHatInstrument &dh, std::string* x) {
	uint8_t source = std::stoi(x->substr(5, 1));
	double sampleRate = std::stod(x->substr(6));
	dh.setClock(source, sampleRate);
}

void wscst(DaqHatInstrument &dh, std::string* x, tcp::acceptor* dataacceptor, tcp::socket* datasocket, thread** scanThread) {
	uint8_t channel_mask = std::stoi(x->substr(5, 1));
	uint32_t options = std::stoi(x->substr(6,2), nullptr, 16);
	uint32_t samples_per_channel = std::stoi(x->substr(8));
	dh.startScan(channel_mask, samples_per_channel, options);

	// This thread is launched by using
    // function pointer as callable
	thread* t = new thread(scan, &dh, dataacceptor, datasocket, samples_per_channel);
	t->detach();
	*scanThread = t;
}

void wscsp(DaqHatInstrument &dh, std::string* x) {
	dh.stopScan();
}

void wsccl(DaqHatInstrument &dh, std::string* x) {
	dh.cleanupScan();
}


void scan(DaqHatInstrument* dh, tcp::acceptor* dataacceptor, tcp::socket* datasocket, uint32_t samples_per_channel) {
	dataacceptor->accept(*datasocket);

	int num_channels = dh->scanChannelCount();

	uint32_t buffer_size = samples_per_channel * num_channels;
	double read_buf[buffer_size];
	int total_samples_read = 0;

	int32_t read_request_size = -1;     // read all available samples
	double timeout = 5.0;

	double scan_rate = 0;
	bool synced = false; // not needed
	uint8_t source = 0; // not needed
		dh->getClock(&source, &scan_rate, &synced);

    uint16_t read_status = 0;
    uint32_t samples_read_per_channel = 0;    int result;

    do
    {
        // Read the specified number of samples.
        result = dh->readScan(&read_status, read_request_size,
            timeout, read_buf, buffer_size, &samples_read_per_channel);
        if (read_status & STATUS_HW_OVERRUN)
        {
            break;
        }
        else if (read_status & STATUS_BUFFER_OVERRUN)
        {
            break;
        }

        total_samples_read += samples_read_per_channel;

        for (uint32_t i = 0; i < samples_read_per_channel; i++) {
            boost::asio::const_buffer buff(&read_buf[i], sizeof(double));
            datasocket->send(buff);
        }

    }
    while ((result == RESULT_SUCCESS) &&
           ((read_status & STATUS_RUNNING) == STATUS_RUNNING) );


}
