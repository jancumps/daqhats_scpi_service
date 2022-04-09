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

uint8_t getChannel(std::string payload) {
	return std::stoi(payload.substr(5, 1));
}

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
					bool enabled = false;
					dh.getIEPE(getChannel(x), &enabled);
     	            x.replace(6,1, enabled ? "1" : "0");
				} else if (x.compare(0, 5, "wiepe") == 0){
					uint8_t enable = std::stoi(x.substr(6, 1));
					dh.setIEPE(getChannel(x), enable);
				} else if (x.compare(0, 5, "rsens") == 0) { // sensitivity
					double sens = 0.0;
					dh.getSensitivity(getChannel(x), &sens);
					std::ostringstream streamObj;
					// Set Fixed -Point Notation
					streamObj << std::fixed;
					//Add double to stream
					streamObj << sens;
					// Get string from output string stream
					std::string d = streamObj.str();
					x.replace(6, d.length(), d);
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

