#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include "frame.h"


/* netbd.h es necesitada por la estructura hostent ;-) */

#define PORT 5040
/* El Puerto Abierto del nodo remoto */

#define MAXDATASIZE 1024
/* El nero mimo de datos en bytes */



pthread_mutex_t lock;


int main(int argc, char *argv[])
{
   int fd, numbytes;
   /* ficheros descriptores */

   char buf[MAXDATASIZE], sensor[1], axis[1];
   /* en donde es almacenarel texto recibido */

   struct hostent *he;
   /* estructura que recibirinformaci sobre el nodo remoto */

   struct sockaddr_in server;
   /* informaci sobre la direcci del servidor */

   reqFrame clietnFrame;
   resFrame serverFrame;
   clietnFrame.preamb = PREAMB;


   if (argc != 2) {
      /* esto es porque nuestro programa so necesitarun argumento, (la IP) */
      if(argv[1] == NULL){
       printf("Utilice: %s  <host>\n",argv[0]);
      }
      exit(1);
   }

   if ((he=gethostbyname(argv[1]))==NULL){
      /* llamada a gethostbyname() */
      printf("gethostbyname() error\n");
      exit(-1);
   }

   if ((fd = socket(AF_INET, SOCK_STREAM, 0))==-1){
      /* llamada a socket() */
      printf("socket() error\n");
      exit(-1);
   }

   server.sin_family = AF_INET;
   server.sin_port = htons(PORT);
   /* htons() es necesaria nuevamente ;-o */
   server.sin_addr = *((struct in_addr *)he->h_addr);
   /*he->h_addr pasa la informaci de ``*he'' a "h_addr" */
   bzero(&(server.sin_zero),8);

   if(connect(fd, (struct sockaddr *)&server,  sizeof(struct sockaddr))==-1){
      /* llamada a connect() */
      printf("connect() error\n");
      exit(-1);
   }
   printf("Connection Established\n");
   while(1){
         printf("For each selection, type the number that corresponds with your petition\n");
         printf("Available sensors:\n 1.-Accelerometer\n 2.-Magnetometer\n 3.-Gyroscope\n 4.-All sensors\n");
         scanf("%d",(int *)&clietnFrame.sensor);

         printf("Available axis:\n 1.- X\n 2.- Y\n 3.- Z\n 4.- XYX\n");
         scanf("%d",(int *)&clietnFrame.axis);

         // printf("Sensor %i\n", clietnFrame.sensor);
         // printf("Axis %i\n", clietnFrame.axis);
    
         pthread_mutex_lock(&lock);

         send(fd,(char *)&clietnFrame,sizeof(reqFrame),0);
      if ((numbytes=recv(fd,(char *)&serverFrame,sizeof(resFrame),0)) == -1){
         /* llamada a recv() */
         printf("Error en recv() \n");
         continue;
      }

      // buf[numbytes]='\0';
      system("clear");
      // printf("%i",numbytes);
      printf("El frame recibido es:\n");
      printf("%x-%x-%x-%x\n",serverFrame.preamb,serverFrame.sensor,serverFrame.size, serverFrame.checksum);
      for(int x = 0; x < sizeof(serverFrame.data); x++){
         printf("%x ",serverFrame.data[x]);
      }
      /* muestra el mensaje de bienvenida del servidor =) */
   
      /*Aqui podríamos poner un scanf para recibir otro servicio a usar */
      pthread_mutex_unlock(&lock);
   
   }
   // close(fd);   /* cerramos fd =) */
   return 0;
}
