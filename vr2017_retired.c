/* Name: Kunal Ostwal
 * unikey: 500668171
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int file_exists(char filename[])
{
    FILE *file;
    char *mode = "rb";
    if ((file = fopen(filename, mode)) == NULL)
    {
        return -1;
    }
    return 1;
}


int is_string_right_length(char *string)
{
    if (strlen(string) != 4)
        return -1;
    return 1;
}


int is_string_hex(char *string)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        if ((string[i] >= '0' && string[i] <= '9') 
            || (string[i] >= 'a' && string[i] <= 'f') 
            || (string[i] >= 'A' && string[i] <= 'F') 
            || (string[i] == 'x'))
            continue;
        else
            return -1;
    }
    return 1;
}


int is_string_start_with_0x(char *string)
{
    if (string[0] == '0' && string[1] == 'x')
        return 1;
    return -1;
}


int find_checksum(int byte0, int byte1, int byte2)
{
    int checksum = byte0 ^ byte1 ^ byte2;
    return checksum;
}


long int file_size_bytes(char filename[])
{
    FILE *file;
    char *mode = "r";
    file = fopen(filename, mode);

    fseek(file, 0, SEEK_END);
    long int size = ftell(file);
    fclose(file);
    return size;
}


int validate_checksum(int packet[5]){
    return  (packet[0] == (packet[1] ^ packet[2] ^ packet[3])) ? 1 : 0;
}


int *coordinates_from_swizzle(int packet[5]){
    static int coordinates[3];
    if (packet[3] == 1){
        coordinates[0] = packet[0];
        coordinates[1] = packet[1];
        coordinates[2] = packet[2];
    } else if (packet[3] == 2){
        coordinates[0] = packet[0];
        coordinates[1] = packet[2];
        coordinates[2] = packet[1];
    } else if (packet[3] == 3){
        coordinates[0] = packet[1];
        coordinates[1] = packet[0];
        coordinates[2] = packet[2];
    } else if (packet[3] == 4){
        coordinates[0] = packet[2];
        coordinates[1] = packet[0];
        coordinates[2] = packet[1];
    } else if (packet[3] == 5){
        coordinates[0] = packet[1];
        coordinates[1] = packet[2];
        coordinates[2] = packet[0];
    } else if (packet[3] == 6){
        coordinates[0] = packet[2];
        coordinates[1] = packet[1];
        coordinates[2] = packet[0];
    }
    
    return coordinates;
    
}


char *return_swizzle(int swizzle){
    if (swizzle == 1){
        return "XYZ";
    } else if (swizzle == 2){
        return "XZY";
    } else if (swizzle == 3){
        return "YXZ";
    } else if (swizzle == 4){
        return "YZX";
    } else if (swizzle == 5){
        return "ZXY";
    } else if (swizzle == 6){
        return "ZYX";
    }
    return "";
}


int main(int argc, char *argv[]){
    
    if (argc > 5){
        printf("Error: Too many command line arguments.\n");
        return -1;
    }
    else if (argc < 5){
        printf("Error: Not enough command line arguments.\n");
        return -1;
    }
    char *filename = argv[1];

    if (file_exists(filename) == -1){
        printf("Error: File %s does not exist!\n", filename);
        return -1;
    }

    for (int i = 2; i < argc; i++){
        if (is_string_start_with_0x(argv[i]) == -1){
            printf("Error: Argument for delimiter byte %d does not begin with 0x\n", i-2);
            return -1;
        }
        if (is_string_right_length(argv[i]) == -1){
            printf("Error: Argument for delimiter byte %d is not of the correct length\n", i-2);
            return -1;
        }
        if (is_string_hex(argv[i]) == -1){
            printf("Error: Argument for delimiter byte %d is not a valid hex value\n", i-2);
            return -1;
        }        
    }

    int byte0 = strtol(argv[2], NULL, 16);
    int byte1 = strtol(argv[3], NULL, 16);
    int byte2 = strtol(argv[4], NULL, 16);

    printf("Delimiter byte 0 is: %d\n", byte0);
    printf("Delimiter byte 1 is: %d\n", byte1);
    printf("Delimiter byte 2 is: %d\n", byte2);

    int checksum = find_checksum(byte0, byte1, byte2);
    printf("Checksum is: %d\n", checksum);


    FILE *binary_file = fopen(filename, "r");
    unsigned int byte;

    const long int file_size = file_size_bytes(filename);
    unsigned char basic_storage[10000];
    for (int i = 0; (i < file_size) || (i < 10000); i++){
        fread(&byte, 1, 1, binary_file);
        basic_storage[i] = byte;
    }

    int chunk_num = 0;
    int byte_num = 0;
    int chunk_counter = 0;
    int chunks[100][640];
    int chunk_size[100];
    int offsets[100];
    memset(offsets, 0, 100*sizeof(int));
    memset(chunk_size, 0, 100*sizeof(int));
    for (int i = 0; i < file_size; i++){

        if (basic_storage[i] == byte0 && 
            basic_storage[i+1] == byte1 && 
            basic_storage[i+2] == byte2 && 
            basic_storage[i+3] == checksum){

            chunk_num++;
           
            offsets[chunk_counter + 1] = i+4;
            chunk_size[chunk_counter] = byte_num;
            byte_num = 0;
            i+=3;
            chunk_counter++;

        }
        else{
            chunks[chunk_counter][byte_num] = basic_storage[i];
            byte_num++;
        }
    }

    chunk_size[chunk_counter] = byte_num;
    int total_chunks = chunk_counter + 1;

    for (int i = 0; i < total_chunks; i++){
        if (chunk_size[i] == 0 && i == total_chunks-1){
            continue;
        }
        printf("\nChunk: %d at offset: %d\n", i, offsets[i]);
        if (chunk_size[i] > 640){
            printf("Error: Chunk size exceeds the maximum allowable chunk size of 640 bytes.\n");
            continue;
        }
        if (chunk_size[i] == 0){
            printf("    No valid packets were found for this chunk.\n");
            continue;
        }
        if (chunk_size[i] % 5 != 0){
            printf("Error: Chunk must be divisible by 5 bytes.\n");
            continue;
        }

        int chunk_packets[128][5];
        memset(chunk_packets, 0, 128*5*sizeof(int));
        int ignored_packets_in_chunk = 0;
        for (int packet_num = 0; packet_num < chunk_size[i]/5; packet_num++){
            int packet[5] = {
                            chunks[i][packet_num*5], 
                            chunks[i][packet_num*5+1], 
                            chunks[i][packet_num*5+2], 
                            chunks[i][packet_num*5+3], 
                            chunks[i][packet_num*5+4]
                            };
            
            if ((packet[0] ^ packet[1] ^ packet[2] ^ packet[3]) != packet[4]){
                printf("    Packet: %d\n", packet_num);
                printf("        Ignoring packet. Checksum was: %d instead of %d.\n", (packet[0] ^ packet[1] ^ packet[2] ^ packet[3]), packet[4]);
                ignored_packets_in_chunk++;                
                continue;
            }

            if (packet[3] < 1 || packet[3] > 6){
                printf("    Packet: %d\n", packet_num);
                printf("        Ignoring packet. Swizzle byte was: %d but can only be between 1 and 6.\n", packet[3]);
                ignored_packets_in_chunk++;
                
                continue;
            }

            int packet_coordinates[3] = {coordinates_from_swizzle(packet)[0], coordinates_from_swizzle(packet)[1], coordinates_from_swizzle(packet)[2]};

            
            printf("    Packet: %d\n", packet_num);
            printf("        Data before swizzle -> B0: %d, B1: %d, B2: %d\n", packet[0], packet[1], packet[2]);
            printf("        Swizzle: %s\n", return_swizzle(packet[3]));
            printf("        Data after swizzle -> X: %d, Y: %d, Z: %d\n", packet_coordinates[0], packet_coordinates[1], packet_coordinates[2]);

            if (packet_num > 0){
                if (abs(packet_coordinates[0] - chunk_packets[packet_num - ignored_packets_in_chunk - 1][0]) > 25){
                    printf("        Ignoring packet. X: %d. Previous valid packet's X: %d. %d > 25.\n", 
                                        packet_coordinates[0], chunk_packets[packet_num - ignored_packets_in_chunk - 1][0], 
                                        abs(packet_coordinates[0] - chunk_packets[packet_num - ignored_packets_in_chunk - 1][0]));
                    ignored_packets_in_chunk++;
                    continue;
                } 
                else if (abs(packet_coordinates[1] - chunk_packets[packet_num - ignored_packets_in_chunk - 1][1]) > 25){
                    printf("        Ignoring packet. Y: %d. Previous valid packet's Y: %d. %d > 25.\n", 
                                        packet_coordinates[1], chunk_packets[packet_num - ignored_packets_in_chunk - 1][1], 
                                        abs(packet_coordinates[1] - chunk_packets[packet_num - ignored_packets_in_chunk - 1][1]));
                    
                    ignored_packets_in_chunk++;
                    continue;
                } 
                else if (abs(packet_coordinates[2] - chunk_packets[packet_num - ignored_packets_in_chunk - 1][2]) > 25){
                    printf("        Ignoring packet. Z: %d. Previous valid packet's Z: %d. %d > 25.\n", 
                                        packet_coordinates[2], chunk_packets[packet_num - ignored_packets_in_chunk - 1][2], 
                                        abs(packet_coordinates[2] - chunk_packets[packet_num - ignored_packets_in_chunk - 1][2]));                    
                    ignored_packets_in_chunk++;
                    continue;
                }
            }    

            chunk_packets[packet_num - ignored_packets_in_chunk][0] = packet_coordinates[0];
            chunk_packets[packet_num - ignored_packets_in_chunk][1] = packet_coordinates[1];
            chunk_packets[packet_num - ignored_packets_in_chunk][2] = packet_coordinates[2];
            chunk_packets[packet_num - ignored_packets_in_chunk][3] = packet_coordinates[3];
            chunk_packets[packet_num - ignored_packets_in_chunk][4] = packet_coordinates[4];

        }

        float avg_x = 0, avg_y = 0, avg_z = 0;
        int packets_in_chunk = chunk_size[i]/5;
        for (int j = 0; j < chunk_size[i]/5 - ignored_packets_in_chunk; j++){

            avg_x += chunk_packets[j][0];
            avg_y += chunk_packets[j][1];
            avg_z += chunk_packets[j][2];
        }

        avg_y /= (packets_in_chunk - ignored_packets_in_chunk);
        avg_x /= (packets_in_chunk - ignored_packets_in_chunk);
        avg_z /= (packets_in_chunk - ignored_packets_in_chunk);

        printf("    Chunk Average X: %.2f, Average Y: %.2f, Average Z: %.2f\n", avg_x, avg_y, avg_z);

        
    }
    printf("\n");
    return 0;
}