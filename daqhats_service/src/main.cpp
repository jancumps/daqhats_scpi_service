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


void testOpen(DaqHatInstrument &dh) {
	// todo: this is a temporary function to put debug code in
	dh.open();
}

void testClose(DaqHatInstrument &dh) {
	// todo: this is a temporary function to put debug code in
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
void rclck(DaqHatInstrument &dh, std::string* x);
void wclck(DaqHatInstrument &dh, std::string* x);

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
				} else if (x.compare(0, 5, "rclck") == 0) { // clock
					rclck(dh, &x);
				} else if (x.compare(0, 5, "wclck") == 0){
					wclck(dh, &x);
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

void rclck(DaqHatInstrument &dh, std::string* x) {
	uint8_t source = 0;
	double sampleRate = 0.0;
	bool synced = false;
	dh.getClock(&source, &sampleRate, &synced);
	x->replace(6, 1, synced ? "1" : "0");
	x->replace(7, 1, std::to_string(source));
	std::string d = formatDouble(sampleRate);
	x->replace(8, d.length(), d);}

void wclck(DaqHatInstrument &dh, std::string* x) {
	uint8_t source = std::stoi(x->substr(6, 1));
	double sampleRate = std::stod(x->substr(7));
	dh.setClock(source, sampleRate);
}
