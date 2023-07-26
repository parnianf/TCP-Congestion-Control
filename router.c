// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <math.h>

#include "common.h"
#include "packet.h"
#include "queue.h"

pthread_mutex_t bufferMutex;

	
#define PORT_ROUTER 8080
#define PORT_B	 8081
#define MAXLINE 1500
#define DROP_PERCENTAGE 10

 
int sockfd, sockfd2;

double maxp = 0.02;         
double tempP = 0;           
double avg = 0;             
int count = -1;             
double weight = 0.003;          
int minThreshold = 5, maxThreshold = 20;

struct sockaddr_in servaddr;
QUEUE* routerBuffer;
int random_drop(int percentage) {
    int randomP = (rand()%100);

    if (randomP < percentage) {
        return -1;
    }

    return 0;
}

int randomEarlyDetection(int s) {
    int res = 0;
    avg = ((1 - weight) * avg) + (weight * size(routerBuffer));

    if(minThreshold <= avg && avg < maxThreshold) {
        count++;
        tempP = ((avg - minThreshold) * maxp)/(maxThreshold - minThreshold);
        double P = tempP/(1 - (count * tempP));
        if(count == 50) {
            P = 1.0;
            res = -1;
        }
        double randomP = (rand()%100)/100.00;
        if(randomP <= P) {
            if(count != 50) {
                res = -1;
            }
            count = 0;
        } else {
            res = 0;
            count = -1;
        }
    } else if(maxThreshold <= avg) {
        res = -1;
        count = 0;
    } else {
        res = 0;
        count = -1;
    }
    return res;
}

void* recieve(void * x){
	while(1){
		char buffer[MAXLINE];
		int len, n;
		len = sizeof(servaddr);
		n = recvfrom(sockfd, buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &servaddr, (socklen_t*)&len);
		srand(time(NULL));
		if(randomEarlyDetection(n) == 0){
			tcp_packet *recvpkt = (tcp_packet *)buffer;
			pthread_mutex_lock(&bufferMutex);
			enqueue(routerBuffer, recvpkt);
			pthread_mutex_unlock(&bufferMutex);				
		}
	}
}

void *sendHandler(void *x){
	while(1){
		if(size(routerBuffer) != 0){
			pthread_mutex_lock(&bufferMutex);
			tcp_packet *recvpkt = peek(routerBuffer);
			dequeue(routerBuffer);
			if(random_drop(DROP_PERCENTAGE) == 0 /*&& size(routerBuffer) < 10*/){////
				int port = recvpkt->hdr.dest;
				struct sockaddr_in clientaddr;
				memset(&clientaddr, 0, sizeof(clientaddr));
				clientaddr.sin_family = AF_INET; 
				clientaddr.sin_addr.s_addr = INADDR_ANY;
				clientaddr.sin_port = htons(port);
				int len = sizeof(clientaddr);
				if(recvpkt->hdr.ctr_flags == ACK){
					sendto(sockfd, recvpkt, TCP_HDR_SIZE, MSG_CONFIRM, (const struct sockaddr *) &clientaddr, len);
				}
				else{
					sendto(sockfd, recvpkt, TCP_HDR_SIZE + get_data_size(recvpkt), MSG_CONFIRM, (const struct sockaddr *) &clientaddr, len);
				}
			}
			pthread_mutex_unlock(&bufferMutex);
		}
	}
}
	
int main() {
	routerBuffer = queueCreate();
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
    if ( (sockfd2 = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket2 creation failed");
		exit(EXIT_FAILURE);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT_ROUTER);
		
	// Bind the socket with the server address
	if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

    pthread_t thread_receiver_A, thread_receiver_B, thread_sender;
    pthread_create(&thread_receiver_A, NULL, sendHandler, NULL);
    pthread_create(&thread_receiver_B, NULL, recieve, NULL);
    pthread_join(thread_receiver_A, NULL);
    pthread_join(thread_receiver_B, NULL);
	return 0;
}
