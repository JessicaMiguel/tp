#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>


int main(int argc, char* argv[])
{
    fflush(stdin);
    unsigned short int protocol_type = 0x1234;
    short int sock_desc=0;

    if ( (sock_desc = socket(AF_PACKET, SOCK_RAW, htons(protocol_type))) < 0)
        printf("\n Error creando el socket\n");
    else
    {
        printf("\n Se creo el socket exitosamente.\n");
        struct sockaddr sock_addrll;
        unsigned char frame_buffer[ETH_FRAME_LEN];
        unsigned char frame_buffer2[] = {};
        socklen_t sock_addrll_len = (socklen_t)sizeof(sock_addrll);

        int buflen;
        buflen = recvfrom(sock_desc, frame_buffer2, ETH_FRAME_LEN, 0x00, (struct sockaddr*) &sock_addrll, (socklen_t*)&sock_addrll_len);
        if ( buflen < 0){
            printf("error\n");
        } 
        else{
            printf("esperando recibir datos\n");
        }
        
        char nombrearchivo[] = {};
        strcpy(nombrearchivo,(frame_buffer2 + sizeof(struct ethhdr)));
        
        printf("fnommm arch = %s\n", nombrearchivo);

        int MF = 1; // Bandera mas fragmentos
        FILE *archivo;
        archivo = fopen(nombrearchivo,"wb");

        int n = 0;
        unsigned char recibido[1500];
        char charMf[2];
        charMf[1] = '\0';

        int tamanoPaquete = 0;
        char chartamanoPaquete[3] = {};


        while(MF){
            printf(" Esperando siguiente paquete \n");
            recvfrom(sock_desc, frame_buffer, ETH_FRAME_LEN, 0x00, (struct sockaddr*) &sock_addrll, &sock_addrll_len);

            chartamanoPaquete[0]= (frame_buffer + sizeof(struct ethhdr))[1];
            chartamanoPaquete[1]= (frame_buffer + sizeof(struct ethhdr))[2];
            chartamanoPaquete[2]= (frame_buffer + sizeof(struct ethhdr))[3];
            chartamanoPaquete[3]= (frame_buffer + sizeof(struct ethhdr))[4];

            tamanoPaquete = atoi(chartamanoPaquete);

            printf("strlen(recibido) %d \n", strlen(recibido));
            for (n = 5; n < tamanoPaquete; n++){
                fwrite(&(frame_buffer + sizeof(struct ethhdr))[n],1,1,archivo);
            }

            printf(" Ultimo elemento, tendria que ser la bandera -> %c \n",recibido[strlen(recibido)-1]);

            charMf[0] = (frame_buffer + sizeof(struct ethhdr))[0];
            MF = atoi(charMf);
            printf("\nMF = %d \n", MF);

            for (int ini = 0; ini < ETH_DATA_LEN; ini++)
                frame_buffer[ini] = '\0';
        }
        fclose(archivo);
        }

return 0;
}
