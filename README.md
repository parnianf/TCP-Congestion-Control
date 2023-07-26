# TCP-Congestion-Control
The aim of this project is to get acquainted with the function of `TCP` and to implement the mechanism of sending correct information along with **congestion control** using `UDP` sockets in the network with the following topology:

<a href='https://www.linkpicture.com/view.php?img=LPic64c19653693f0523396033'><img src='https://www.linkpicture.com/q/Screenshot-4748.png' type='image' width=60%></a>


In this topology, computer `A` sends a relatively large file to computer `B` through router `R`. Router `R` has a buffer in which incoming messages are stored and sent to the destination in `FIFO` form. To implement this exercise, consider each of computers `A` and `B` and router `R` as an independent process. To send the file, the computer sends it in 1.5 KB packets through the **sliding-window** mechanism. `UDP-type` socket is used for the mentioned inter-process communication.

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

<div>
<a href='https://www.linkpicture.com/view.php?img=LPic64c198206c894916902525'><img src='https://www.linkpicture.com/q/Screenshot-4751.png' type='image' width=40%></a>
<a href='https://www.linkpicture.com/view.php?img=LPic64c19753d885f1458394023'><img src='https://www.linkpicture.com/q/Screenshot-4750.png' type='image' width=40%></a>
<div>

## Report
The report is available [here](https://github.com/parnianf/TCP-Congestion-Control/blob/main/CA4_Report.pdf).

<br>

### Contributors
* [Parnian Fazel](https://github.com/parnianf/)
* [Paria Khoshtab](https://github.com/Theparia/)
