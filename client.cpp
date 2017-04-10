/*                                                               Assignment 2  
    MEMBER 1 : 
        NAME : Chinmaya Pancholi
        ROLL NUMBER : 13CS30010

    MEMBER 2 : 
        NAME : Aishik Chakraborty
        ROLL NUMBER : 13CS30041
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h> 
#include <netinet/in.h> 
#include <string.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>

using namespace std;

int port_num = 23465;

#define BUF_SIZE 10000

#define FILE_SIZE 10000
char *file_data;

#define NUM_ROWS 1000
char* file_rows[NUM_ROWS];

int main ()
{
    //cout<<"Enter the IP of server\n";
    //char serv_IP1[100];
    //cin>>serv_IP1;

    file_data = (char *)malloc (FILE_SIZE*sizeof(char));
    int rst; 
    int cfd;

    int sfd;
    sfd = socket (AF_INET, SOCK_STREAM, 0);

    if (sfd == -1)
    {
        perror ("1. Client: socket error");
    }

    printf ("Socket fd = %d\n", sfd);
    

    struct sockaddr_in srv_addr, cli_addr;
    socklen_t addrlen = sizeof (struct sockaddr_in);
    
    memset (&srv_addr, 0, addrlen);
    
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_port   = htons (port_num);
    //srv_addr.sin_addr.s_addr = inet_addr(serv_IP1);
    //bzero(&srv_addr.sin_zero, 8);    

    rst = inet_pton (AF_INET, "127.0.0.1", &srv_addr.sin_addr); 

    if (rst <= 0)
    {
        perror ("2. Client Presentation to network address conversion2.\n");
    }    
    
    printf ("Trying to connect to server\n");

    rst = connect (sfd, (struct sockaddr *) &srv_addr, addrlen);
    if (rst == -1)
    {
        perror ("3. Client: Connect failed.");
    }

    printf ("Connected to server\n");
    
    char buf[BUF_SIZE] = {'\0'}; 
    
    printf ("Sending message.\n");
    

    char file_to_read[1000];
    cout<<"Please enter file_name\n";       //getting the name of file from the agent
    scanf("%s",file_to_read);

    FILE *fp1;

    fp1 = fopen(file_to_read, "r");

    int fr_ret = fread(file_data, sizeof(file_data[0]), FILE_SIZE, fp1);

    char* row1;
    int row_count = 0;
    char timestamp[100];
    char final_msg_to_serv[BUF_SIZE];

    char final_msg_to_write[BUF_SIZE];

    cout<<file_data<<"\n";

        char dest[200];
        strcpy(dest,file_to_read);
        //sprintf(dest,"%d",getpid());
        strcat(dest, "_Ticket.csv");

        char booking[BUF_SIZE];

    bzero(final_msg_to_write, BUF_SIZE);

    while((row1 = strsep(&file_data, "\n")) != NULL)
    {
        bzero(booking, BUF_SIZE);
     
        if(strcmp(row1, "") != 0)
        {
            cout<<row1<<"\n";
            file_rows[row_count++] = row1;
        }
        else
        {
            break;
        }
        bzero(timestamp, 100);
        sprintf(timestamp, "%u\n", (unsigned)time(NULL));

        bzero(final_msg_to_serv, 10000);
        strcpy(final_msg_to_serv, file_rows[row_count-1]);
        strcat(final_msg_to_serv,",");
        strcat(final_msg_to_serv,timestamp);

        rst = send (sfd, final_msg_to_serv, BUF_SIZE, 0);
        if (rst == -1)
        {
            perror ("4. Client: Send failed");
        }

        printf ("Message sent successfully.\n");

        int bytesReceived;
        if((bytesReceived = recv(sfd, booking, BUF_SIZE, 0)) > 0)
        {
            cout<<"Booking received - > "<<booking<<"\n";
        }
        else
        {
            perror("Client: Receive Failed");
        }

        strcat(final_msg_to_write,booking);

        //FILE *fp2;

        //fp2 = fopen(dest, "a");

        //strcat(booking,"\0");
        
        //int ret_fw = fwrite(booking, sizeof(booking[0]), sizeof(booking)/sizeof(booking[0]), fp2);

        //fclose(fp2);

        sleep(5);
    }

        FILE *fp2;

        fp2 = fopen(dest, "w");

        strcat(booking,"\0");
        
        int ret_fw = fwrite(final_msg_to_write, sizeof(final_msg_to_write[0]), strlen(final_msg_to_write), fp2);        //output the data received in the ticket file

        fclose(fp2);


    /*printf ("Waiting to receive a message.\n");
    rst = recv (sfd, buf, BUF_SIZE, 0);
    if (rst == -1)
    {
        perror ("Client: Receive failed");
        exit (1);
    }
    
    printf ("Received message = |%s|\n", buf);*/
    
    rst = close (sfd);
    if (rst == -1)
    {
        perror ("Client close failed");
        exit (1);
    }    
    
    return 0;
}


