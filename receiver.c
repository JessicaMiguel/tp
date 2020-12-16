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
    unsigned short int protocol_type = 0x1234	; //0x0800
    short int sock_desc=0;

    if ( (sock_desc = socket(AF_PACKET, SOCK_RAW, htons(protocol_type))) < 0)
        printf("\n Error creando el socket\n");
    else
    {
        printf("\n Socket listo y esperando\n");
        struct sockaddr_ll sock_addrll;
        unsigned char frame_buffer[ETH_FRAME_LEN];
        unsigned char frame_buffer2[] = {};
        socklen_t sock_addrll_len = (socklen_t)sizeof(sock_addrll);

        recvfrom(sock_desc, frame_buffer2, ETH_FRAME_LEN, 0x00, (struct sockaddr*) &sock_addrll, &sock_addrll_len);
        char fileName[] = {};
        strcpy(fileName,(frame_buffer2 + sizeof(struct ethhdr)));

        printf("Archivo que llega = %s\n", fileName);

        int MF = 1; 
        FILE *archivo;
        archivo = fopen(fileName,"wb");

        int n = 0;
        unsigned char recibido[1500];
        char charMf[2];
        charMf[1] = '\0';

        int frameSize = 0;
        char charframeSize[3] = {};


        while(MF){
            printf("Recibiendo... \n");
            recvfrom(sock_desc, frame_buffer, ETH_FRAME_LEN, 0x00, (struct sockaddr*) &sock_addrll, &sock_addrll_len);

            charframeSize[0]= (frame_buffer + sizeof(struct ethhdr))[1];
            charframeSize[1]= (frame_buffer + sizeof(struct ethhdr))[2];
            charframeSize[2]= (frame_buffer + sizeof(struct ethhdr))[3];
            charframeSize[3]= (frame_buffer + sizeof(struct ethhdr))[4];

            frameSize = atoi(charframeSize);

            printf("%d \n", strlen(recibido));
            for (n = 5; n < frameSize; n++){
                fwrite(&(frame_buffer + sizeof(struct ethhdr))[n],1,1,archivo);
            }

            printf(" %c \n",recibido[strlen(recibido)-1]);

            charMf[0] = (frame_buffer + sizeof(struct ethhdr))[0];
            MF = atoi(charMf);
            printf("\n%d \n", MF);

            for (int ini = 0; ini < ETH_DATA_LEN; ini++)
                frame_buffer[ini] = '\0';
        }
        fclose(archivo);
        }

return 0;
}
