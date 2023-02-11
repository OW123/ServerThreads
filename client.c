#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


/* netbd.h es necesitada por la estructura hostent ;-) */

#define PORT 5040
/* El Puerto Abierto del nodo remoto */

#define MAXDATASIZE 1024
/* El nero mimo de datos en bytes */



int main(int argc, char *argv[])
{
   int fd, numbytes;
   /* ficheros descriptores */

   char buf[MAXDATASIZE];
   /* en donde es almacenarel texto recibido */

   struct hostent *he;
   /* estructura que recibirinformaci sobre el nodo remoto */

   struct sockaddr_in server;
   /* informaci sobre la direcci del servidor */

   if (argc != 3) {
      /* esto es porque nuestro programa so necesitarun argumento, (la IP) */
      if(argv[2] == NULL){
          printf("Falta parametro de servicio a usar\n");
       }
      else if(argv[1] == NULL){
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
         send(fd,argv[2],100,0);
      if ((numbytes=recv(fd,buf,MAXDATASIZE,0)) == -1){
         /* llamada a recv() */
         printf("Error en recv() \n");
         exit(-1);
      }

      buf[numbytes]='\0';
      system("clear");
      // printf("%i",numbytes);
      printf("Mensaje del Servidor:\n %s\n",buf);
      /* muestra el mensaje de bienvenida del servidor =) */

      printf("Servicio seleccionado: %s\n",argv[2]);
   
      /*Aqui podríamos poner un scanf para recibir otro servicio a usar */
   
   }

   close(fd);   /* cerramos fd =) */

}
