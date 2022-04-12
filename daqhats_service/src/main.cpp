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
#include <boost/asio.hpp>


#include <stdlib.h>

#include "DaqHatInstrument.h"

using boost::asio::ip::tcp;


void testCode(DaqHatInstrument &dh) {
	// todo: this is a temporary function to put debug code in
	dh.open();
	dh.setIEPE(0, true);
	dh.setIEPE(0, false);
	dh.close();
}


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
void wscst(DaqHatInstrument &dh, std::string* x);
void wscsp(DaqHatInstrument &dh, std::string* x);
void wsccl(DaqHatInstrument &dh, std::string* x);

int main(int argc, char** argv) {
	int portno = 0;

	try {
		if (argc != 3) {
			std::cerr << "Usage: port daqaddress" << std::endl;
			return 1;
		}

		portno = atoi(argv[1]);
		boost::asio::io_service io_service;
		tcp::endpoint endpoint(tcp::v4(), (portno));
		tcp::acceptor acceptor(io_service, endpoint);

		DaqHatInstrument dh(atoi(argv[2]));
		dh.open();

		for (;;) {
			std::string x;
			tcp::iostream stream;
			acceptor.accept(*stream.rdbuf());
			while (stream >> x) {
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
				} else if (x.compare(0, 5, "wscst") == 0){ // scan start
					wscst(dh, &x);
				} else if (x.compare(0, 5, "wscsp") == 0){ // scan stop
					wscsp(dh, &x);
				} else if (x.compare(0, 5, "wsccl") == 0){ // scan clean
					wsccl(dh, &x);
				}

				// send reply
				stream << x; // the endl is passed on from the source. << std::endl;
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

void wscst(DaqHatInstrument &dh, std::string* x) {
	uint8_t channel_mask = std::stoi(x->substr(5, 1));
	uint32_t options = std::stoi(x->substr(6,2), nullptr, 16);
	uint32_t samples_per_channel = std::stoi(x->substr(8));
	dh.startScan(channel_mask, samples_per_channel, options);
}

void wscsp(DaqHatInstrument &dh, std::string* x) {
	dh.stopScan();
}

void wsccl(DaqHatInstrument &dh, std::string* x) {
	dh.cleanupScan();
}
