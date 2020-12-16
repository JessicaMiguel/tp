#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

// Estructura de los componentes del Frame del eth
union ethframe
{
  struct
  {
    struct ethhdr    header;
    unsigned char    data[ETH_DATA_LEN];
  } field;
  unsigned char    buffer[ETH_FRAME_LEN];
};

int main(int argc, char **argv) {
    //Definicion de la informacion
    char *iface =  "wlp1s0";//"enp2s0";//"enp2s0";//"wlp3s0"; // 
    unsigned char MAC_destino[ETH_ALEN] //
        = {  0x28,0x39,0x26,0xd5,0x86,0x1d}; /*0x01,0x00,0x5e,0x01,0x01,0x01/*
                                            0xff,0xff,0xff,0xff,0xff,0xff/*
                                            0x28,0x39,0x26,0xd5,0x86,0x1d*/
    unsigned short proto = 0x1234;/*0x0800	;custom protocol del git*/
    // Creacion Socket
    int s;

    if ((s = socket(AF_PACKET, SOCK_RAW, htons(proto))) < 0) {
        perror("Error al abrir el socket");
        exit(1);
    }else{
        printf("Socket listo\n");
    }

    struct ifreq buffer;
    int ifindex;
    memset(&buffer, 0x00, sizeof(buffer));
    strncpy(buffer.ifr_name, iface, IFNAMSIZ);

    
    if (ioctl(s, SIOCGIFINDEX, &buffer) < 0){ //con el nombre de la if se busca el indice
        perror("Error al obtener el indice");
    }else{
        printf("\nIndice = %d\n", buffer.ifr_ifindex);
    }
    ifindex = buffer.ifr_ifindex;
    unsigned char source[ETH_ALEN];

    //buffer.ifr_flags |= IFF_MULTICAST;
    //buffer.ifr_flags |= IFF_BROADCAST;
   
    if (ioctl(s, SIOCGIFHWADDR, &buffer) < 0){ //esto para la mac q envia
        perror("Error al obtener la direccion MAC\n");
    }
    else{
        printf("Obtuve la MAC de origen\n");
    }
    memcpy((void*)source, (void*)(buffer.ifr_hwaddr.sa_data),ETH_ALEN);

    
    union ethframe frame;
    memcpy(frame.field.header.h_dest, MAC_destino, ETH_ALEN);
    memcpy(frame.field.header.h_source, source, ETH_ALEN);
    frame.field.header.h_proto = htons(proto);


    struct sockaddr_ll saddrll;
    memset((void*)&saddrll, 0, sizeof(saddrll));
    saddrll.sll_family = PF_PACKET;
    saddrll.sll_ifindex = ifindex;
    saddrll.sll_halen = ETH_ALEN;
    saddrll.sll_pkttype = /*PACKET_HOST | PACKET_MULTICAST |*/ PACKET_BROADCAST;
    memcpy((void*)(saddrll.sll_addr), (void*)MAC_destino, ETH_ALEN);

    
    unsigned char data[3];
    unsigned short data_len2 = strlen(data);
    unsigned int frame_len2 = data_len2 + ETH_HLEN;

    
    int n = 5;
    int flag = 0;
    unsigned long data_len1 = 0;
    unsigned long longitud_data = 0;
    unsigned int frame_len1 = 0;
    unsigned char data_inicializada[ETH_DATA_LEN];

    //Obtengo el nombre del archivo
    for (int i=0; i < strlen(argv[1]); i++){
        frame.field.data[i] = argv[1][i];
    }

    data_len1 = strlen(frame.field.data);
    frame_len1 = data_len1 + ETH_HLEN;

    if (sendto(s, frame.buffer, frame_len1, 0, (struct sockaddr*)&saddrll, sizeof(saddrll)) > 0)
            
            printf("Sending...\n");
        else
            printf("Error, no se pudo enviar\n");


    FILE *arch;
    arch = fopen(argv[1],"rb");
    fscanf(arch,"%c",&frame.field.data[n]);

    while (!feof(arch)){
        n++;
        if (flag == 1){
            flag = 0;
        }

        if  (n >= ETH_DATA_LEN - 5){
            frame.field.data[1] = '1';
            frame.field.data[2] = '4';
            frame.field.data[3] = '9';
            frame.field.data[4] = '5';
            frame.field.data[0] = '1';// flag

            data_len1 = 1500;
            frame_len1 = data_len1 + ETH_HLEN;

            
            if (sendto(s, frame.buffer, frame_len1, 0, (struct sockaddr*)&saddrll, sizeof(saddrll)) > 0)
                printf("Sending... x2\n");
            else
                printf("Error, no se pudo enviar\n");

            
            for (int ini = 0; ini < ETH_DATA_LEN; ini++)
                frame.field.data[ini] = '\0';

            n = 5;
            flag = 1;
        }
        fscanf(arch,"%c",&frame.field.data[n]);
    }
    if (flag == 0){
        int fileSize = n;

        sprintf(data,"%d",fileSize);
        longitud_data = strlen(data);
        if (longitud_data == 4){
            frame.field.data[1] = data[0];
            frame.field.data[2] = data[1];
            frame.field.data[3] = data[2];
            frame.field.data[4] = data[3];
        }else {
            if (longitud_data == 3){
                frame.field.data[1] = '0';
                frame.field.data[2] = data[0];
                frame.field.data[3] = data[1];
                frame.field.data[4] = data[2];
            }else {
                if (longitud_data == 2){
                    frame.field.data[1] = '0';
                    frame.field.data[2] = '0';
                    frame.field.data[3] = data[0];
                    frame.field.data[4] = data[1];
                } else{
                    frame.field.data[1] = '0';
                    frame.field.data[2] = '0';
                    frame.field.data[3] = '0';
                    frame.field.data[4] = data[0];
                }
            }
        }
        frame.field.data[0] = '0';
        data_len1 = n;
        frame_len1 = data_len1 + ETH_HLEN;

        if (sendto(s, frame.buffer, frame_len1, 0, (struct sockaddr*)&saddrll, sizeof(saddrll)) > 0)
            
            printf("Archivo enviado = %s\n", argv[1]);
        else
            printf("Error, no se pudo enviar\n");
    }
    fclose(arch);
    return 0;
}
