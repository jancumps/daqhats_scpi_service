/*
 ============================================================================
 Name        : linux_scpi_socket.c
 Author      : Jan Cumps
 Version     :
 Copyright   : free for all
 Description :
 ============================================================================
 */

/* A simple server in the internet domain using TCP
 The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


#include "scpi/scpi.h"
#include "scpi-def.h"

#include "linux_scpi_socket.h"

int ss_socket_desc, ss_client_sock, cs_socket_desc, cs_client_sock;


void sig_handler(int signo) {
  if (signo == SIGINT) {
    shutdown(ss_socket_desc, SHUT_RDWR);

  }
}

/** SCPI utilities
 *
 */

size_t SCPI_Write(scpi_t * context, const char * data, size_t len) {
  (void) context;
  int n = write(ss_client_sock, data, len);
  if (n < 0)
    fprintf(stderr, "ERROR writing to socket\r\n");
  return n;
}

scpi_result_t SCPI_Flush(scpi_t * context) {
  (void) context;

  return SCPI_RES_OK;
}

int SCPI_Error(scpi_t * context, int_fast16_t err) {
  (void) context;

  fprintf(stderr, "**ERROR: %d, \"%s\"\r\n", (int16_t) err,
      SCPI_ErrorTranslate(err));
  return 0;
}

scpi_result_t SCPI_Control(scpi_t * context, scpi_ctrl_name_t ctrl,
    scpi_reg_val_t val) {
  (void) context;

  if (SCPI_CTRL_SRQ == ctrl) {
    fprintf(stderr, "**SRQ: 0x%X (%d)\r\n", val, val);
  } else {
    fprintf(stderr, "**CTRL %02x: 0x%X (%d)\r\n", ctrl, val, val);
  }
  return SCPI_RES_OK;
}

scpi_result_t SCPI_Reset(scpi_t * context) {
  (void) context;

  fprintf(stderr, "**Reset\r\n");
  return SCPI_RES_OK;
}

scpi_result_t SCPI_SystemCommTcpipControlQ(scpi_t * context) {
  (void) context;

  return SCPI_RES_ERR;
}

/*
 * the below code is adapted from BinaryTides
 Server and client example with C sockets on Linux
 https://www.binarytides.com/server-client-example-c-sockets-linux/
 by Silver Moon https://plus.google.com/117145272367995638274
 */

#include<stdio.h>
#include<string.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write

int main(int argc, char *argv[]) {
  int c, read_size, ss_portno, cs_portno;
  struct sockaddr_in ss_server, ss_client, cs_server;
  char ss_client_message[2000];

  signal(SIGINT, sig_handler);

  SCPI_Init(&scpi_context, scpi_commands, &scpi_interface, scpi_units_def,
  SCPI_IDN1, SCPI_IDN2, SCPI_IDN3, SCPI_IDN4, scpi_input_buffer,
      SCPI_INPUT_BUFFER_LENGTH, scpi_error_queue_data,
      SCPI_ERROR_QUEUE_SIZE);

  if (argc < 3) {
    fprintf(stderr, "ERROR, no ports provided\n");
    exit(1);
  }
  ss_portno = atoi(argv[1]);
  cs_portno = atoi(argv[2]);


  // server side socket initialisation

  //Create socket
  ss_socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if (ss_socket_desc == -1) {
    printf("Could not create server socket");
  }
  puts("Server socket created");

  //Prepare the sockaddr_in structure
  ss_server.sin_family = AF_INET;
  ss_server.sin_addr.s_addr = INADDR_ANY;
  ss_server.sin_port = htons(ss_portno);

  //Bind
  if (bind(ss_socket_desc, (struct sockaddr *) &ss_server, sizeof(ss_server)) < 0) {
    //print the error message
    perror("Server socket bind failed. Error");
    return 1;
  }
  puts("Server socket bind done");

  // server side socket created


// client socket to connect to the instrument


  //Create socket
  cs_socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (cs_socket_desc == -1)
  {
      printf("Could not create client socket");
  }
  puts("Client socket created");

  cs_server.sin_addr.s_addr = inet_addr("127.0.0.1");
  cs_server.sin_family = AF_INET;
  cs_server.sin_port = htons( cs_portno );

  //Connect to remote server
  if (connect(cs_socket_desc , (struct sockaddr *)&cs_server , sizeof(cs_server)) < 0)
  {
      perror("connect to instrument service failed. Error");
      return 1;
  }

  puts("Connected to instrument service\n");



// client side connected to the instrument






  while (1) {


    //Listen
    listen(ss_socket_desc, 3);  // I may have to reduce the queue to 1

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    ss_client_sock = accept(ss_socket_desc, (struct sockaddr *) &ss_client,
        (socklen_t*) &c);
    if (ss_client_sock < 0) {
      perror("accept failed");
      return 1;
    }
    puts("Connection accepted");

    //Receive a message from client
    while ((read_size = recv(ss_client_sock, ss_client_message, 2000, 0)) > 0) {
      SCPI_Input(&scpi_context, ss_client_message, strlen(ss_client_message));
      memset(ss_client_message, 0x00, strlen(ss_client_message)); // clean any garbage after eating the SCPI instructions - otherwise a \r\n may cause an additional, dirty call to the SCPI lib
    }

    if (read_size == 0) {
      puts("Client disconnected");
      fflush(stdout);
    } else if (read_size == -1) {
      perror("recv failed");
    }
    close(ss_client_sock);
  }

  close(ss_client_sock);

  return 0;
}


int sendToInstrument(char *buffer, size_t size) {
  //Send instruction to instrument

  buffer[size-1] = '\n';
  if( send(cs_socket_desc , buffer , size , 0) < 0)
//  if( send(cs_socket_desc , "hello, world!\n" , 14 , 0) < 0)
  {
      puts("Send to instrument service failed");
      return 1;
  }

  //Receive a reply from the server
  if( recv(cs_socket_desc , buffer , size , 0) < 0)
  {
      puts("Receive from instrument service failed");
      return 1;
  }

//  puts("Server reply :");
//  puts(cs_server_message);
  return 0;

}
