/*
This is a prototype developed by researchers from Universidade Presbiteriana Mackenzie (Brazil)
This program communicate with a wearable prototype.
A signal is send to a serial port where the VEST prototype is connected.
The VEST-1.1 uses a 3x3 motors array located at the back of the vest using 2 Arduinos.
The pattern of the signal is a string starting with "<" and ending with ">"
Considering "�" = FF = 255, to activate all the motors the signal that should be send is "<���������>"

The program shows an interface with buttons to send patterns to the serial port.
Each button send a specific pattern to the VEST. 
The goal is to experiment simple and complex patterns recognition by the person using the VEST.
*/

#include <iostream>
#include <stdio.h>
#include <fcntl.h>  /* File Control Definitions          */
#include <termios.h>/* POSIX Terminal Control Definitions*/
#include <unistd.h> /* UNIX Standard Definitions         */
#include <errno.h>  /* ERROR Number Definitions          */

using namespace std;

void send(char data[], size_t size);
bool writeDataToSerial(string port, char data[], size_t size);
void getPattern(char* out, bool* values);
void getOff(char* out);
void getHorizontalLine(char* out);
void getVerticalLine(char* out);
void getDiagonalLeft(char* out);
void getDiagonalRight(char* out);
void getL(char* out);
void getU(char* out);
void getT(char* out);
void getSquare(char* out);

bool isFinish = false;
// 1 millisecond = 0.001 microseconds
unsigned int interval = 500000; // 500 milliseconds

int main(int argc, const char** argv)
{
	return 0;
}

void getPattern(char* out, bool* values) {
	out[0] = '<';
	for(int i=0; i < 10; ++i){
    	out[i+1] = (values[i] ? char(255) : char(1));
  	}
	out[11] = '>';
	out[12] = '\0';
}

void getOff(char* out) {
	// 0 0 0
	// 0 0 0
	// 0 0 0
	bool x[10] = {false, false, false, false, false, false, false, false, false, false};
	getPattern(out, x);
}

void getHorizontalLine(char* out) {
	// 0 0 0
	// x x x
	// 0 0 0
	bool x[10] = {false, false, false, true, true, true, false, false, false, false};
	getPattern(out, x);
}

void getVerticalLine(char* out) {
	// 0 x 0
	// 0 x 0
	// 0 x 0
	bool x[10] = {false, true, false, false, true, false, false, true, false, false};
	getPattern(out, x);
}

void getDiagonalLeft(char* out) {
	// x 0 0
	// 0 x 0
	// 0 0 x
	bool x[10] = {true, false, false, false, true, false, false, false, true, false};
	getPattern(out, x);
}

void getDiagonalRight(char* out) {
	// 0 0 x
	// 0 x 0
	// x 0 0
	bool x[10] = {false, false, true, false, true, false, true, false, false, false};
	getPattern(out, x);
}

void getL(char* out) {
	// x 0 0
	// x 0 0
	// x x x
	bool x[10] = {true, false, false, true, false, false, true, true, true, false};
	getPattern(out, x);
}

void getU(char* out) {
	// x 0 x
	// x 0 x
	// x x x
	bool x[10] = {true, false, true, true, false, true, true, true, true, false};
	getPattern(out, x);
}

void getT(char* out) {
	// x x x
	// 0 x 0
	// 0 x 0
	bool x[10] = {true, true, true, false, true, false, false, true, false, false};
	getPattern(out, x);
}

void getSquare(char* out) {
	// x x x
	// x 0 x
	// x x x
	bool x[10] = {true, true, true, true, false, true, true, true, true, false};
	getPattern(out, x);
}

void send(char* data, size_t size) {
	writeDataToSerial("/dev/ttyACM0", data, size);
	usleep(interval);
}

/* The data cannot contain zero char values.
When 0x00, send 0x01 to avoid serial communication issues. */
bool writeDataToSerial(string port, char data[], size_t size) {
	int fd;
	fd = open(port.c_str(), O_RDWR | O_NOCTTY);
	if(fd == 1)
		printf("\n  Error! in Opening %s\n", port.c_str());
	else
		printf("\n %s Opened Successfully\n", port.c_str());

	struct termios SerialPortSettings;	/* Create the structure                          */

	tcgetattr(fd, &SerialPortSettings);	/* Get the current attributes of the Serial port */

	cfsetispeed(&SerialPortSettings,B9600); /* Set Read  Speed as 9600                       */
	cfsetospeed(&SerialPortSettings,B9600); /* Set Write Speed as 9600                       */

	SerialPortSettings.c_cflag &= ~PARENB;   /* Disables the Parity Enable bit(PARENB),So No Parity   */
	SerialPortSettings.c_cflag &= ~CSTOPB;   /* CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit */
	SerialPortSettings.c_cflag &= ~CSIZE;	 /* Clears the mask for setting the data size             */
	SerialPortSettings.c_cflag |=  CS8;      /* Set the data bits = 8                                 */

	SerialPortSettings.c_cflag &= ~CRTSCTS;       /* No Hardware flow Control                         */
	SerialPortSettings.c_cflag |= CREAD | CLOCAL; /* Enable receiver,Ignore Modem Control lines       */ 
	
	
	SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);          /* Disable XON/XOFF flow control both i/p and o/p */
	SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);  /* Non Cannonical mode                            */

	SerialPortSettings.c_oflag &= ~OPOST;/*No Output Processing*/

	if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0) /* Set the attributes to the termios structure*/
		printf("\n  ERROR ! in Setting attributes");
	else
		printf("\n  BaudRate = 9600 \n  StopBits = 1 \n  Parity   = none");
		
	/*------------------------------- Write data to serial port -----------------------------*/
	//char write_buffer[] = "<ÿÿÿÿÿÿÿÿÿÿ>";	/* Buffer containing characters to write into port	     */	
	//char write_buffer[] = "<>";
	int  bytes_written  = 0;  	/* Value for storing the number of bytes written to the port */ 

	bytes_written = write(fd, data, size);
	printf("\n  %s written to %s", data, port.c_str());
	printf("\n  %d Bytes written to %s", bytes_written, port.c_str());
	printf("\n +----------------------------------+\n\n");

	close(fd);/* Close the Serial port */

	//return Status;
	return true;
}

#include <boost/python.hpp>

BOOST_PYTHON_MODULE(vest)
{
    using namespace boost::python;
    def("send", send);
}