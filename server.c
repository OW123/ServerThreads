/* Estos son los ficheros de cabecera usuales */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>
#include <pthread.h>

#define BACKLOG 5 /* El numero de conexiones permitidas */

#define MAXDATASIZE 1024

pthread_t threads[BACKLOG];
pthread_mutex_t lock;

void *handle_client(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[MAXDATASIZE];
    int read_size;
    
   pthread_mutex_lock(&lock);

    while((read_size = recv(sock , buffer , MAXDATASIZE , 0)) > 0) {
        printf("Received message: %s\n", buffer);
        send(sock , buffer , strlen(buffer) , 0);
    }
    
    if(read_size == 0)
        printf("Client disconnected\n");
    else
        perror("recv failed");
        
   pthread_mutex_unlock(&lock);
   close(sock);
    
    return 0;
}


int main( int argc, char *argv[]){


   int  PORT =5040; /* El puerto que sera abierto */

   int fd, fd2,fdser; /* los ficheros descriptores */

   int position, current_clients = 0;

   char buf[MAXDATASIZE], bufser[MAXDATASIZE];

   struct sockaddr_in server;
   /* para la informaci de la direcci del servidor */

   struct sockaddr_in client;
   /* para la informaci de la direcci del cliente */

   int sin_size;
   if(argc < 2)
    {
      printf("utilice %s <Puerto> \n",argv[0]);
      exit(1);
    }
   /* A continuaci la llamada a socket() */
   if ((fd=socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
      printf("error en socket()\n");
      exit(-1);
   }
  else
     PORT=atoi(argv[1]);

   server.sin_family = AF_INET;

   server.sin_port = htons(PORT);
   /* Recuerdas a htons() de la secci "Conversiones"? =) */

   server.sin_addr.s_addr = INADDR_ANY;
   /* INADDR_ANY coloca nuestra direcci IP automicamente */

   bzero(&(server.sin_zero),8);
   /* escribimos ceros en el reto de la estructura */

  /*puts("bind");*/
   /* A continuaci la llamada a bind() */
   if(bind(fd,(struct sockaddr*)&server,
           sizeof(struct sockaddr))==-1) {
      printf("error en bind() \n");
      exit(-1);
   }
 /* puts("listen");*/
   if(listen(fd,BACKLOG) == -1) {  /* llamada a listen() */
      printf("error en listen()\n");
      exit(-1);
   }
   /* puts("ciclo");*/
   while(1) {
         sin_size=sizeof(struct sockaddr_in);
      /* puts("sizeof");*/
         /* A continuaci la llamada a accept() */
         if ((fd2 = accept(fd,(struct sockaddr *)&client, &sin_size))==-1) {
            printf("error en accept()\n");
            exit(-1);
         }
         
         /* Inicializando variable mutex*/
         pthread_mutex_init(&lock,NULL);

         if (pthread_create(&threads[current_clients++], NULL, handle_client, (void*)&fd2) < 0) {
               error("ERROR creating thread");
            }

    for (int i = 0; i < BACKLOG; i++) {
        if (pthread_join(threads[i], NULL) < 0) {
            error("ERROR joining thread");
        }
    }
   }
    //  printf("Se obtuvo una conexi desde %s\n", inet_ntoa(client.sin_addr) );
      /* que mostrarla IP del cliente */

      // recv(fd2,buf,MAXDATASIZE,0);
      // if(strcmp(buf,"Help") == 0){
      //     send(fd2,"Servicios:\n Fecha y Hora (Ingresa 'Hora')\n Calendario Mensual(Ingresa 'Calendario')\n Fase Lunar (Ingresa 'Luna')\n Tiempo\n",300,0);
      // }
      // else if(strcmp(buf,"Hora") == 0){
      //     system("date>serv");
      //     fdser = open("serv",O_RDONLY);
      //     position =  read(fdser,bufser,MAXDATASIZE);
      //     bufser[position] = '\0';
      //     printf("Hora es:\n %s\n",bufser);
      //     send(fd2,bufser,500000,0);
      // }
      // else if(strcmp(buf,"Calendario") == 0){
      //     system("cal>serv");
      //     fdser = open("serv",O_RDONLY);
      //     position = read(fdser,bufser,MAXDATASIZE);
      //     bufser[position] = '\0';
      //     printf("%s\n",bufser);
      //     send(fd2,bufser,MAXDATASIZE,0);
      // }
      // else if(strcmp(buf,"Luna") == 0){
      //     system("curl wttr.in/moon>serv");
      //     fdser = open("serv",O_RDONLY);
      //     position = read(fdser,bufser,MAXDATASIZE);
      //     bufser[position] = '\0';
      //     printf("%s\n",bufser);
      //     send(fd2,bufser,MAXDATASIZE,0);
      // }
      // else if(strcmp(buf,"Tiempo") == 0){
      //     system("curl wttr.in>serv");
      //     fdser = open("serv",O_RDONLY);
      //     position = read(fdser,bufser,MAXDATASIZE);
      //     perror("");
      //     bufser[position] = '\0';
      //     printf("%s\n",bufser);
      //     send(fd2,bufser,MAXDATASIZE,0);
      //     perror("");
      // }
      // else{
      //   send(fd2,"Comando no reconocido. Usa 'Help' para ver los comandos posibles\n",300,0);
      // }

      // printf("Servicio: %s\n",buf);
      // //send(fd2,"Bienvenido a mi servidor.\n",100,0);
      // /* que enviarel mensaje de bienvenida al cliente */

      close(fd2); /* cierra fd2 */
      return 0;
   }