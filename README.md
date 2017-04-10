# Ticket_Counter_Using_Socket_Programming
This project simulates a railway ticket booking system where several ticket booking agents (clients) simultaneously book tickets by sending requests to the central server. 

## Implementation Details: 
 
### Server : 
The server would accept multiple requests from different booking agents (clients) and handle those requests through `select()`. Timestamps are used to by server while processing multiple simultaneous requests. The server would have an option (press a button, say ‘1’) to display the current booking status of all the trains.

### Booking Agent: 
The booking agent would read the booking details from a csv file where every line is a separate booking request. The booking agent would wait for 5 seconds after every booking request.

### Tasks: 
1. Implement a server 

2. Implement booking agent process.  

3. Berth allocation policy 

4. Display system in server  
 
5. Prioritize booking which has (i) more number of passengers and (ii) more number of senior citizens.



