# TCP-Congestion-Control
The aim of this project is to get acquainted with the function of `TCP` and to implement the mechanism of sending correct information along with **congestion control** using `UDP` sockets in the network with the following topology:

<a href='https://www.linkpicture.com/view.php?img=LPic64c19653693f0523396033'><img src='https://www.linkpicture.com/q/Screenshot-4748.png' type='image' width=60%></a>

## Go Back N Protocol (GBN)
Go-Back-N ARQ is a specific instance of the automatic repeat request (ARQ)
protocol, in which the sending process continues to send a number of frames
specified by a window size even without receiving an acknowledgment (ACK)
packet from the receiver.


## Selective Repeat Protocol (SR)
Selective repeat protocol, also called Selective Repeat ARQ (Automatic Repeat request), is a data link layer protocol that uses a sliding window method for
the reliable delivery of data frames. Here, only the erroneous or lost frames are
retransmitted, while the good frames are received and buffered.

## Random Early Drop
Rather than wait for queue to become full, drop each arriving packet with some drop probability whenever the queue length exceeds some drop level.
Here is the algorithm of RED:


<a href='https://www.linkpicture.com/view.php?img=LPic64c198206c894916902525'><img src='https://www.linkpicture.com/q/Screenshot-4751.png' type='image' width=40%></a>

<a href='https://www.linkpicture.com/view.php?img=LPic64c19753d885f1458394023'><img src='https://www.linkpicture.com/q/Screenshot-4750.png' type='image' width=40%></a>


## Report
The report is available [here](https://github.com/parnianf/TCP-Congestion-Control/blob/main/CA4_Report.pdf).
