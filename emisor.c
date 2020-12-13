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
    char *iface =  "ens3";//"wlp1s0";//"enp2s0";//"wlp3s0"; // 2 Nombre de la interface de red
    unsigned char MAC_destino[ETH_ALEN] // Cargo valores MAC destino
        = { 0x01, 0x00, 0x5E, 0x00,0x00,0x00/*0x28,0x39,0x26,0xd5,0x86,0x1d/*0xFF,0xFF,0xFF,0xFF,0xFF,0xFF/* 0x52,0x54,0x00,0x6a,0x95,0x20/*0xF4, 0x8E, 0x38, 0xF6, 0xA5, 0xEB/*0x14, 0xDD, 0xA9, 0x02, 0xC9, 0x3C/*0xF4, 0x8E, 0x38, 0xF6, 0xA5, 0xEB/0x50, 0x7B, 0x9D, 0x7A, 0x2E, 0x56/0xF4, 0x6D, 0x04, 0xCD, 0xC8, 0x09/*0x48, 0xE2, 0x44, 0x81, 0xA6, 0xDF/*0x14, 0xDD, 0xA9, 0x02, 0xC9, 0x3C/*0x48, 0xE2, 0x44, 0x81, 0xA6, 0xDF/*0xEC, 0xA8, 0x6B, 0x52, 0x12, 0xD8/0x24, 0x0A, 0x64, 0x0F, 0x74, 0x2A*//*0x48, 0xE2, 0x44, 0x81, 0xA6, 0xDF*/ };
    unsigned short proto = 0x0080; // Campo de protocolo de 16 bits

    // Creacion Socket
    int s;

    if ((s = socket(AF_PACKET, SOCK_RAW, htons(proto))) < 0) {
        perror("Error al abrir el socket");
        exit(1);
    }else{
        printf("Exito al abrir el socket");
    }

    struct ifreq buffer;
    int ifindex;
    memset(&buffer, 0x00, sizeof(buffer));
    strncpy(buffer.ifr_name, iface, IFNAMSIZ);

    // Obtenemos indice
    if (ioctl(s, SIOCGIFINDEX, &buffer) < 0){
        perror("Error al obtener el indice");
    }else{
        printf("\nSe obtuvo el indice\n");
    }
    ifindex = buffer.ifr_ifindex;
    unsigned char source[ETH_ALEN];

    // Obtenemos direccion MAC
    if (ioctl(s, SIOCGIFHWADDR, &buffer) < 0){
        perror("Error al obtener la direccion MAC\n");
    }
    else
    {
        printf("\nSe obtuvo la MAC\n");
    }
    
    memcpy((void*)source, (void*)(buffer.ifr_hwaddr.sa_data),ETH_ALEN);



    //Rellenamos el contenido del paquete
    union ethframe frame;
    memcpy(frame.field.header.h_dest, MAC_destino, ETH_ALEN);
    memcpy(frame.field.header.h_source, source, ETH_ALEN);
    frame.field.header.h_proto = htons(proto);


    // Rellenamos la estructura sockaddr_ll
    struct sockaddr_ll saddrll;
    memset(&saddrll, 0, sizeof(saddrll));
    saddrll.sll_family = PF_PACKET;
    saddrll.sll_ifindex = ifindex;
    saddrll.sll_halen = ETH_ALEN;
    memcpy((void*)(saddrll.sll_addr), (void*)MAC_destino, ETH_ALEN);



    //Variables MF
    unsigned char data[3];
    unsigned short data_len2 = strlen(data);
    unsigned int frame_len2 = data_len2 + ETH_HLEN;

    // Variables Paquete
    int n = 5;
    int bandera = 0;
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
            printf("Se realizo el envio del paquete exitosamente\n");
        else
            printf("Error, no se pudo enviar\n");

    /* Calculo tamano en bytes del archivo */
    FILE *archivo1;
    archivo1 = fopen(argv[1],"rb");
    fscanf(archivo1,"%c",&frame.field.data[n]);

    while (!feof(archivo1)){
        n++;
        if (bandera == 1){
            bandera = 0;
        }

        if  (n >= ETH_DATA_LEN - 5){
            frame.field.data[1] = '1';
            frame.field.data[2] = '4';
            frame.field.data[3] = '9';
            frame.field.data[4] = '5';
            frame.field.data[0] = '1';// Bandera

            data_len1 = 1500;
            frame_len1 = data_len1 + ETH_HLEN;

            // Realizo envio
            if (sendto(s, frame.buffer, frame_len1, 0, (struct sockaddr*)&saddrll, sizeof(saddrll)) > 0)
                printf("El paquete se envio correctamente.\n");
            else
                printf("Error, no se pudo enviar\n");

            //Inicializo
            for (int ini = 0; ini < ETH_DATA_LEN; ini++)
                frame.field.data[ini] = '\0';

            n = 5;
            bandera = 1;
        }
        fscanf(archivo1,"%c",&frame.field.data[n]);
    }
    if (bandera == 0){
        int tamanoArchivo = n;

        sprintf(data,"%d",tamanoArchivo);
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
            printf("Se realizo el envio del paquete exitosamente\n");
        else
            printf("Error, no se pudo enviar\n");
    }
    //close(s);
    fclose(archivo1);
    return 0;
}
