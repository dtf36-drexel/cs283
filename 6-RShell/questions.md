1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

Client waits for an end-of-message marker like an EOF character. It loops on recv() until that marker is detected so all data can be recieved.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

The protocol should define boundaries using unique delimiters or length headers. Otherwise messages could be fragmented or cut short and be misread.

3. Describe the general differences between stateful and stateless protocols.

Stateful protocols keep session context across messages. Stateless treat each request separately without remembering past interactions.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

It's useful for when low latency and overhead are critical, and occasional packet loss is acceptable (i.e. gaming, streaming)

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The OS provides socket interfaces that abstract details of the network communication
