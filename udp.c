#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SEND_NODATA      0
#define SEND_HEARTBEAT   1

#define PACKET_DEVICEID  0
#define PACKET_BATTLEVEL 1
#define PACKET_TYPE      2
#define PACKET_SEQUENCE  3
#define PACKET_SIZE      4

int openListener(uint16_t port)
{
   int s;
   struct sockaddr_in server;
   socklen_t namelen = sizeof(server);
   
   s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
   if(s == -1) {
      printf("ERROR: Creating socket\n");
      return 0;
   }
      
   memset(&server, 0, sizeof(server));
   server.sin_family = AF_INET;
   server.sin_addr.s_addr = htonl(INADDR_ANY);
   server.sin_port = htons(port);

   if(bind(s, (struct sockaddr *)&server, namelen) == -1) {
      printf("ERROR: Bind socket\n");
      close(s);
      s = -1;
      return s;
   }

   return s;
}

void closeListener(int s)
{
   close(s);
}

void printData(unsigned char *data)
{
   printf("DEVICE ID: %d BATT: %d TYPE: ", data[PACKET_DEVICEID], data[PACKET_BATTLEVEL]);

   switch(data[PACKET_TYPE]) {
      case SEND_NODATA:
         printf("[SEND_NODATA] ");
         break;
      case SEND_HEARTBEAT:
         printf("[SEND_HEARTBEAT] ");
         break;
      default:
         printf("[DEFAULT] ");
         break;
   }
   
   printf("SEQ: %d\n", data[PACKET_SEQUENCE]);
}

void readListener(int s)
{
    int i = 0;
    int returnValue;
    unsigned char data[PACKET_SIZE];

    for (i = 0; i < 1000; i++) {
        memset(data, 0 , PACKET_SIZE);
        
        returnValue = recv(s, data, PACKET_SIZE, 0);
        if (-1 == returnValue) {
            printf("WARNING: Receive failed\n");
            continue;
        }
        
        if (0 == returnValue) {
            printf("WARNING: Server closed connection\n");
            continue;
        }
        
        printData(data);
    }
}

int main(int argc, const char * argv[]) {
   int s;
   
   printf("Opening port 1234 to receive broadcasts\n");
   
   s = openListener(1234);
   if(s == -1) {
      printf("ERROR: opening port 1234\n");
      return 1;
   }
   
   readListener(s);
   closeListener(s);

    return 0;
}
