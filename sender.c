#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "packet.h"
#include "common.h"
#include "queue.h"
#define max(x,y) (((x) >= (y)) ? (x) : (y))

#define STDIN_FD 0
#define RETRY 200 //milli second
#define PORT_ROUTER 8080
#define PORT_B 8081

int transmission_round = 0; //keep track of the round
int next_seqno; // next byte to send
int exp_seqno;  // expected byte to be acked
int send_base = 0;  // first byte in the window
float window_size = 50; // window size at the beginning of slow start
int portno;
int timer_on = 0;
FILE *fp;

int sockfd, serverlen;
struct sockaddr_in serveraddr, senderAddr;
struct itimerval timer;
tcp_packet *recvpkt;
sigset_t sigmask;

void send_packet(char* buffer, int len, int seqno);
void resend_packets(int sig);
void init_timer(int delay, void (*sig_handler)(int));
void start_timer();
void stop_timer();

void send_packet(char* buffer, int len, int seqno) {
    tcp_packet *sndpkt = make_packet(len);
    memcpy(sndpkt->data, buffer, len);
    sndpkt->hdr.seqno = seqno;
    sndpkt->hdr.src = portno;
    sndpkt->hdr.dest = PORT_B;
    printf("Sending packet of sequence number %d of data size %d to %s\n", seqno, len, inet_ntoa(serveraddr.sin_addr));
    if (sendto(sockfd, sndpkt, TCP_HDR_SIZE + get_data_size(sndpkt), 0,
                       (const struct sockaddr *)&serveraddr, serverlen) < 0)
    {
        error("sendto");
    }
    free(sndpkt);
}


void resend_packets(int sig)
{
    char buffer[DATA_SIZE];
    int len;

    if (sig == SIGALRM)
    {
        VLOG(INFO, "Timeout happened");
        
        //resend all packets range between send_base and next_seqno

        for (int i = send_base; i < next_seqno; i += DATA_SIZE)
        {
            // locate the pointer to be read at next_seqno
            fseek(fp, i, SEEK_SET);
            // read bytes from fp to buffer
            len = fread(buffer, 1, DATA_SIZE, fp);
            // send pkt
            send_packet(buffer, len, i);
        }
    }
}

void start_timer()
{
    sigprocmask(SIG_UNBLOCK, &sigmask, NULL);
    setitimer(ITIMER_REAL, &timer, NULL);
    timer_on = 1;
    // printf("Timer on\n");
}

void stop_timer()
{
    sigprocmask(SIG_BLOCK, &sigmask, NULL);
    timer_on = 0;
    // printf("Timer off\n");
}

/*
 * init_timer: Initialize timeer
 * delay: delay in milli seconds
 * sig_handler: signal handler function for resending unacknoledge packets
 */
void init_timer(int delay, void (*sig_handler)(int))
{
    signal(SIGALRM, resend_packets);
    timer.it_interval.tv_sec = delay / 1000; // sets an interval of the timer
    timer.it_interval.tv_usec = (delay % 1000) * 1000;
    timer.it_value.tv_sec = delay / 1000; // sets an initial value
    timer.it_value.tv_usec = (delay % 1000) * 1000;

    sigemptyset(&sigmask);
    sigaddset(&sigmask, SIGALRM);
}

int main(int argc, char **argv)
{
    char *hostname;
    char buffer[DATA_SIZE];
    int len;
    int dup_cnt; // count of continuous duplicate ACKs

    FILE* plot; // file that contains plot of window_size, ssthresh and time
    // time_t now, start;
    struct timeval start, now;

    /* check command line arguments */
    if (argc != 4)
    {
        fprintf(stderr, "usage: %s <hostname> <port> <FILE>\n", argv[0]);
        exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);
    fp = fopen(argv[3], "r");
    if (fp == NULL)
    {
        error(argv[3]);
    }

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");


    memset(&senderAddr, 0, sizeof(senderAddr));
    senderAddr.sin_family = AF_INET; // IPv4
	senderAddr.sin_addr.s_addr = INADDR_ANY;
	senderAddr.sin_port = htons(portno);
		
	// Bind the socket with the server address
	if (bind(sockfd, (const struct sockaddr *)&senderAddr, sizeof(senderAddr)) < 0){
		perror("bind failed");
		exit(EXIT_FAILURE);
	}


    /* initialize server server details */
    bzero((char *)&serveraddr, sizeof(serveraddr));
    serverlen = sizeof(serveraddr);

    /* covert host into network byte order */
    if (inet_aton(hostname, &serveraddr.sin_addr) == 0)
    {
        fprintf(stderr, "ERROR, invalid host %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(PORT_ROUTER);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    assert(MSS_SIZE - TCP_HDR_SIZE > 0);

    // init timer
    init_timer(RETRY, resend_packets);

    next_seqno = 0;
    exp_seqno = DATA_SIZE;

    dup_cnt = 1;

    plot = fopen("CWND.csv", "w+");
    gettimeofday(&start,NULL);
    clock_t start_time = clock();
    while (1)
    {
        gettimeofday(&now,NULL);

        // send all pkts in the effective window
        while (next_seqno < send_base + (int)(window_size) * DATA_SIZE)
        {
            // start the timer if not alr started
            if (timer_on == 0) start_timer();

            // printf("current send_base: %d \n", send_base);

            // locate the pointer to be read at next_seqno
            fseek(fp, next_seqno, SEEK_SET);
            // read bytes from fp to buffer
            len = fread(buffer, 1, DATA_SIZE, fp);
            // if end of file
            if (len <= 0)
            {
                VLOG(INFO, "End Of File read");
                send_packet(buffer, 0, 0);
                break;
            }

            // send pkt
            send_packet(buffer, len, next_seqno);

            // increment the next sequence number to be sent
            next_seqno += len;
        }

        // wait for ACK
        int n = recvfrom(sockfd, buffer, MSS_SIZE, MSG_WAITALL, (struct sockaddr *)&serveraddr, (socklen_t *)&serverlen);
        
        // make recv pkt
        recvpkt = (tcp_packet *)buffer;
        assert(get_data_size(recvpkt) <= DATA_SIZE);

        // if receive ack for last pkt
        if (recvpkt->hdr.ackno % DATA_SIZE > 0) {
            printf("***All packets sent successfully\n");
            clock_t duration = clock() - start_time;
            double time_taken = ((double)duration)/CLOCKS_PER_SEC; // calculate the elapsed time
            printf("------------------------------------------>Time taken: %f secconds\n", time_taken);
            stop_timer();
            break;
        }

        if (recvpkt->hdr.ackno == send_base) {
            dup_cnt += 1;
        }

        if (exp_seqno <= recvpkt->hdr.ackno)
        {
            send_base = recvpkt->hdr.ackno;
            dup_cnt = 1; // reset count for dup ACKs 
            exp_seqno = send_base + DATA_SIZE;
            stop_timer();
            // start timer for unacked pkts
            if (send_base < next_seqno)
                start_timer();
        }
    }
    fclose(plot);
    return 0;
}
