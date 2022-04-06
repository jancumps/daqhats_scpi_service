/*
 * main.cpp
 *
 *  Created on: 06 apr. 2022
 *      Author: jancu
 */

#include <ctime>
#include <iostream>
#include <string>
#include <boost/asio.hpp>


#include <stdlib.h>

#include "DaqHatInstrument.h"

using boost::asio::ip::tcp;


void testCode() {
	// todo: this is a temporary function to put debug code in
	DaqHatInstrument dh(0);
	dh.open();
	dh.setIEPE(0, true);
	dh.setIEPE(0, false);
	dh.close();
}

int main(int argc, char** argv) {

	testCode(); // todo remove
	return 0; // todo remove


  int portno = 0;


  try {
    if (argc != 2) {
      std::cerr << "Usage: port" << std::endl;
      return 1;
    }

    portno = atoi(argv[1]);

    boost::asio::io_service io_service;

    tcp::endpoint endpoint(tcp::v4(), (portno));
    tcp::acceptor acceptor(io_service, endpoint);


    for (;;) {
      std::string x;
      tcp::iostream stream;
      acceptor.accept(*stream.rdbuf());
      while (stream >> x) {

        // process the commands





        // send reply

        stream << x; // the endl is passed on from the source. << std::endl;
      }
    }
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}

