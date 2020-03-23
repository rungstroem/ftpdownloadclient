#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>

const int SIZE = 1024;          // Size of Buffers
char receiveBuff[SIZE];         // Buffer to send to the server
char sendBuff[SIZE];            // Buffer to receive from server
char pasvBuff[] = "pasv";       // Buffer to see if PASV Command was entered
char quitBuff[] = "QUIT";       // Buffer to see if QUIT Command was entered
char userLogin[] = "USER ";
char passLogin[] = "PASS ";
char username[] = "ftpuser";
char password[] = "12345678";
char trfType[] = "TYPE I";
char getBuff[] = "GET ";
char fileName[] = "arnbcfg.xml";

#define FTP_PORT 21
#define FTP_ADDR 192.168.1.10

int main(){
    int length = 0, i=0;
    int a1, a2, a3, a4, p1, p2, dataPort;			//PASV Information

    //Add Server Information
    struct sockaddr_in server_addr;					//Internet address of server
    memset(&server_addr, 0, sizeof(server_addr));   //Clear structure
    server_addr.sin_family = AF_INET;				//Set address typedef
    memcpy(&server_addr.sin_addr, FTP_ADDR, sizeof(FTP_ADDR));
    server_addr.sin_port = htons(FTP_PORT);			//Use FTP port

    //Create control command socket to connect to FTP server
    int commSock;
	if((commSock = socket(AF_INET, SOCK_STREAM, 0)) < 0){ //Command socket in client
		//Implement error handling here
		std::cout << "failed to create command socket\n";
	}
    //Connect to FTP server on port 21
    if((connect(commSock, (struct sockaddr *)&server_addr, sizeof(server_addr))) < 0){
		//Implement error handling here
		std::cout << "failed to connect to socket\n";
	}

    //Get connection succesful message
    if((read(commSock, receiveBuff, sizeof(receiveBuff)-1)) < 0){
		//Implement error handling here
		std::cout << "failed for read from socket\n";
	}
	write(fileno(stdout), receiveBuff, sizeof(receiveBuff)-1);	//Write server response to stdout
    //Implement error handling on servers response

	//Create login information
	strcat(userLogin, username);
	strcat(userLogin, "\r\n");
	strcat(passLogin, password);
	strcat(passLogin, "\r\n");

	//Write login information to server
	memset(receiveBuff, 0, SIZE);
	write(commSock, userLogin, strlen(userLogin));
	if((read(commSock, receiveBuff, sizeof(receiveBuff)-1)) <0){
		//Implement error handling
		std::cout << "failed reading from socket after username send\n";
	}
	memset(receiveBuff, 0, SIZE);
	write(commSock, passLogin, strlen(passLogin));
	if((read(commSock, receiveBuff, sizeof(receiveBuff)-1)) <0){
		//Implement error handling
		std::cout << "failed reading from socket efter password send\n";
	}
	write(fileno(stdout), receiveBuff, sizeof(receiveBuff)-1);	//Write server response to stdout
	//Implement error handling on server response here
	//Maybe server responds with "230" - standard response for sufficient credentials to grant access to server

	/*	A valid connection should be established now
		The commSock is made to send commands to the server */

    //Clear buffers
    memset(receiveBuff, 0, SIZE);
    memset(sendBuff, 0, SIZE);

    if((send(commSock, pasvBuff, strlen(pasvBuff) , 0)) < 0){		//Send pasv command to server
		//Implement error handling here
		std::cout << "failed to write PASV command to server\n";
	}

    if((read(commSock, receiveBuff, sizeof(receiveBuff) - 1)) < 0){	//Read Response from Server
		std::cout << "failed to read from server after PASV command send\n";
	}
	write(fileno(stdout), receiveBuff, sizeof(receiveBuff) - 1);    //Print Response from Server to screen
	//Implement error handling on server response 

    //If "227" response from server upon sending PASV then create data socket - server ready
    if(strncmp("227", receiveBuff, 3) == 0){
        sscanf(receiveBuff, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &a1,&a2,&a3,&a4,&p1,&p2);
        dataPort = (p1 * 256) + p2;

        struct sockaddr_in server_addr2;					//Internet address of server
        memset(&server_addr2, 0, sizeof(server_addr2));     //Clear structure
        server_addr2.sin_family = AF_INET;						//Set address typedef
        memcpy(&server_addr2.sin_addr, FTP_ADDR, sizeof(FTP_ADDR));
        server_addr.sin_port = htons(dataPort);				//Use FTP data port

        //Create data transfer socket
        int dataSock;                               //Data socket in client
        if((dataSock = socket(PF_INET, SOCK_STREAM, 0)) < 0){
			//Implement error handling here
            std::cout << "failed to create data socket - client error\n";
        }

        //Connect to FTP server on dataport
        if((connect(dataSock, (struct sockaddr *)&server_addr2, sizeof(server_addr2))) < 0){
            //Implement error handling here
			std::cout << "failed to connect to data socket - client error\n";
        }
		
		//Get connect succesful message
        if((read(dataSock, receiveBuff, sizeof(receiveBuff) - 1)) < 0){
			//Implement error handling here
			std::cout << "failed to read from socket\n";
		}
        write(fileno(stdout), receiveBuff, sizeof(receiveBuff) - 1);	//Write server response to stdout
		
		memset(receiveBuff, 0, SIZE);	//Clear receiveBuff

		//Set data transfer type - ie. binary
		write(commSock, trfType, sizeof(trfType));
		if((read(commSock, receiveBuff, sizeof(receiveBuff) - 1)) < 0){
			//Implement error handling here
			std::cout << "failed to read from socket after data type set\n";
		}
		write(fileno(stdout), receiveBuff, sizeof(receiveBuff) - 1);	//Write server response to stdout
		
		memset(receiveBuff, 0, size);	//Clear receiveBuff

		//Create command with file name to transfer
		strcat(getBuff, fileName);
		strcat(getBuff, "\r\n");

		write(commSock, getBuff, strlen(sendBuff));
		if((read(commSock, receiveBuff, sizeof(receiveBuff))) < 0){
			//Implement error handling here
			cout << "Failed to read from command socket after GET command send\n";
		}

		read(dataSock, receiveBuff, sizeof(receiveBuff));

		ofstream xmlfile;
		xmlfile.open(fileName);
		xmlfile << receiveBuff;
		xmlfile.close();

		close(dataSock);
		close(commSock);
    }

	//Clean up
	free(server_addr);
	free(server_addr2);

    return 0;
}
