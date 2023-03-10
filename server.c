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
#include <json-c/json.h>
#include "frame.h"

#define BACKLOG 5 /* El numero de conexiones permitidas */
#define MAXDATASIZE 1024
#define SENSOR_FAIL 254
#define SIZE_FAIL 0
#define CRC_FAIL 255


pthread_t threads[BACKLOG];
pthread_mutex_t lock;

int accele_data[3];
int magne_data[3];
int gyro_data[3];






void *handle_client(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[MAXDATASIZE];
    int read_size;
    int sensor;
    int axis;
    char *token;

   reqFrame clientFrame;
   resFrame serverFrame;

   serverFrame.preamb = PREAMB;

   pthread_mutex_lock(&lock);

    while((read_size = recv(sock ,(char *)&clientFrame , sizeof(reqFrame) , 0)) > 0) {
        printf("Received message sensor: %i\t axis:%i\n", clientFrame.sensor, clientFrame.axis);

         if(clientFrame.sensor == 1){
            serverFrame.sensor = clientFrame.sensor;
            switch(clientFrame.axis){
               case 1: 
                  serverFrame.data[0] = accele_data[0];
                  serverFrame.size = 5;
                  break;
               case 2: 
                  serverFrame.data[0] = accele_data[1];
                  serverFrame.size = 5;
                  break;
               case 3:
                  serverFrame.data[0] = accele_data[2];
                  serverFrame.size = 5;
                  break;
               case 4:
                  serverFrame.data[0] = accele_data[0];
                  serverFrame.data[1] = accele_data[1];
                  serverFrame.data[2] = accele_data[2];
                  serverFrame.size = 7;                  
            }
         }else if(clientFrame.sensor == 2){
            serverFrame.sensor = clientFrame.sensor;
            switch(clientFrame.axis){
               case 1: 
                  serverFrame.data[0] = magne_data[0];
                  serverFrame.size = 5;
                  break;
               case 2:
                  serverFrame.data[1] = magne_data[1];
                  serverFrame.size = 5;
                  break;
               case 3:
                  serverFrame.data[2] = magne_data[2];
                  serverFrame.size = 5;
                  break;
               case 4:
                  serverFrame.data[0] = magne_data[0];
                  serverFrame.data[1] = magne_data[1];
                  serverFrame.data[2] = magne_data[2];
                  serverFrame.size = 7;                  
                  break;               
            }
         }else if(clientFrame.sensor  == 3){
            serverFrame.sensor = clientFrame.sensor;
            switch(clientFrame.axis){
               case 1:
                  serverFrame.data[0] = gyro_data[0];
                  serverFrame.size = 5;
                  break;
               case 2:
                  serverFrame.data[0] = gyro_data[1];
                  serverFrame.size = 5;
                  break;
               case 3:
                  serverFrame.data[0] = gyro_data[2];
                  serverFrame.size = 5;
                  break;
               case 4:
                  serverFrame.data[0] = gyro_data[0];
                  serverFrame.data[1] = gyro_data[1];
                  serverFrame.data[2] = gyro_data[2];
                  serverFrame.size = 7;
                  break;
            }
         }else if(clientFrame.sensor  == 4){
            serverFrame.sensor = clientFrame.sensor;
            switch(clientFrame.axis){
               case 1:
                  serverFrame.data[0] = accele_data[0];
                  serverFrame.data[1] = magne_data[0];
                  serverFrame.data[2] = gyro_data[0];
                  serverFrame.size = 7;
                  break;
               case 2:
                  serverFrame.data[0] = accele_data[1];
                  serverFrame.data[1] = magne_data[1];
                  serverFrame.data[2] = gyro_data[1];
                  serverFrame.size = 7;
                  break;
               case 3:
                  serverFrame.data[0] = accele_data[2];
                  serverFrame.data[1] = magne_data[2];
                  serverFrame.data[2] = gyro_data[2];
                  serverFrame.size = 7;
                  break;
               case 4:
                  serverFrame.data[0] = accele_data[0];
                  serverFrame.data[1] = accele_data[1];
                  serverFrame.data[2] = accele_data[2];
                  serverFrame.data[3] = magne_data[0];
                  serverFrame.data[4] = magne_data[1];
                  serverFrame.data[5] = magne_data[2];
                  serverFrame.data[6] = gyro_data[0];
                  serverFrame.data[7] = gyro_data[1];
                  serverFrame.data[8] = gyro_data[2];
                  serverFrame.size = 13;
                  break;
            }
         }else{
            /**When reporting the error, the sensor field on the response frame should be 0xFE, data size as 0 and 
            CS as 0xFF. */
            serverFrame.sensor = SENSOR_FAIL;
            serverFrame.size = SIZE_FAIL;;
            serverFrame.checksum = CRC_FAIL;
         }
         // printf("Data to send: %i\n", serverFrame.data);

        send(sock ,(char *)&serverFrame, sizeof(resFrame) , 0);
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

   /*Declaraciones para lectura JSON*/
   FILE *fp;
	char bufferJson[1024];

	struct json_object *parsed_json;
	struct json_object *acceletomer_json;
	struct json_object *magnetometer_json;;
	struct json_object *gyroscope_json;

    struct json_object *acceletomer;
    struct json_object *magnetometer;
    struct json_object *gyroscope;


	// size_t n_friends;
    size_t n_acceletomers;
    size_t n_magnets;
    size_t n_gyroscope;

	size_t i;



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
            continue;
         }
         
         /* Inicializando variable mutex*/
         pthread_mutex_init(&lock,NULL);

         /*Lectura JSON*/
         fp = fopen("data.json","r");
	      fread(bufferJson, 1024, 1, fp);
	      fclose(fp);

	      parsed_json = json_tokener_parse(bufferJson);
         json_object_object_get_ex(parsed_json, "accelerometer", &acceletomer_json);
         json_object_object_get_ex(parsed_json, "magnetometer", &magnetometer_json);
         json_object_object_get_ex(parsed_json, "gyroscope", &gyroscope_json);


         n_acceletomers = json_object_array_length(acceletomer_json);
         n_magnets = json_object_array_length(magnetometer_json);
         n_gyroscope = json_object_array_length(gyroscope_json);

      for(i = 0; i < n_acceletomers; i++){
         acceletomer = json_object_array_get_idx(acceletomer_json, i);
         magnetometer = json_object_array_get_idx(magnetometer_json, i);
         gyroscope = json_object_array_get_idx(gyroscope_json, i);


         printf("%lu. %i\n",i + 1, json_object_get_int(acceletomer));
         printf("%lu. %i\n",i + 1, json_object_get_int(magnetometer));
         printf("%lu. %i\n",i + 1, json_object_get_int(gyroscope));


         accele_data[i] = json_object_get_int(acceletomer);
         magne_data[i] = json_object_get_int(magnetometer);
         gyro_data[i] = json_object_get_int(gyroscope);
      }


         if (pthread_create(&threads[current_clients++], NULL, handle_client, (void*)&fd2) < 0) {
               perror("ERROR creating thread");
               continue;
            }

      for (int i = 0; i < BACKLOG; i++) {
         if (pthread_join(threads[i], NULL) < 0) {
               perror("ERROR joining thread");
               continue;
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