/* 
Name: Kunal Ostwal
SID: 500668171
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int bitwise_xor_of_three_numbers(int a, int b, int c) {
    return a ^ b ^ c;
}

int * return_swizzle_bytes(int x, int y, int z, int swizzle) {
    static int coordinates[3];
    switch (swizzle) {
    case 1:
        coordinates[0] = x;
        coordinates[1] = y;
        coordinates[2] = z;
        printf("        Swizzle: XYZ\n");
        break;
    case 2:
        coordinates[0] = x;
        coordinates[1] = z;
        coordinates[2] = y;
        printf("        Swizzle: XZY\n");
        break;
    case 3:
        coordinates[0] = y;
        coordinates[1] = x;
        coordinates[2] = z;
        printf("        Swizzle: YXZ\n");
        break;
    case 4:
        coordinates[0] = z;
        coordinates[1] = x;
        coordinates[2] = y;
        printf("        Swizzle: YZX\n");
        break;
    case 5:
        coordinates[0] = y;
        coordinates[1] = z;
        coordinates[2] = x;
        printf("        Swizzle: ZXY\n");
        break;
    case 6:
        coordinates[0] = z;
        coordinates[1] = y;
        coordinates[2] = x;
        printf("        Swizzle: ZYX\n");
        break;
    default:
        printf("        Ignoring packet. Swizzle byte was: %d but can only be between 1 and 6.\n", swizzle);
    }
    return coordinates;
}

int is_string_start_with_0x(char * string) {
    if (string[0] == '0' && string[1] == 'x')
        return 1;
    return -1;
}

int is_string_right_length(char * string) {
    if (strlen(string) != 4)
        return -1;
    return 1;
}

int is_string_hex(char * string) {
    int i;
    for (i = 0; i < 4; i++) {
        if ((string[i] >= '0' && string[i] <= '9') ||
            (string[i] >= 'a' && string[i] <= 'f') ||
            (string[i] >= 'A' && string[i] <= 'F') ||
            (string[i] == 'x'))
            continue;
        else
            return -1;
    }
    return 1;
}

int chunk_processor(int chunk[128][5], int chunk_num, int offset, int packets_in_chunk, int byte_count) {

    printf("Chunk: %d at offset: %d\n", chunk_num, offset);

    if (packets_in_chunk > 128) {
        printf("Error: Chunk size exceeds the maximum allowable chunk size of 640 bytes.\n");
        return 0;
    }

    if (byte_count != 0) {

        printf("Error: Chunk must be divisible by 5 bytes.\n\n");
        return -1;
    }

    if (packets_in_chunk == 0) {
        printf("    No valid packets were found for this chunk.\n\n");
        return 0;
    }

    int valid_packets[128][3];
    int previous_valid_packet[3] = {
        0,
        0,
        0
    };

    int ignored_packets = 0;

    for (int packet_num = 0; packet_num < packets_in_chunk; packet_num++) {
        int packet_checksum = chunk[packet_num][0] ^ chunk[packet_num][1] ^ chunk[packet_num][2] ^ chunk[packet_num][3];
        printf("    Packet: %d\n", packet_num);

        if (packet_checksum != chunk[packet_num][4]) {
            printf("        Ignoring packet. Checksum was: %d instead of %d.\n", packet_checksum, chunk[packet_num][4]);
            ignored_packets++;
            continue;
        }

        if (chunk[packet_num][3] > 6 || chunk[packet_num][3] < 1) {
            printf("        Ignoring packet. Swizzle byte was: %d but can only be between 1 and 6.\n", chunk[packet_num][3]);
            ignored_packets++;
            continue;
        }

        printf("        Data before swizzle -> B0: %d, B1: %d, B2: %d\n", chunk[packet_num][0],
            chunk[packet_num][1],
            chunk[packet_num][2]);
        int * coordinates = return_swizzle_bytes(chunk[packet_num][0],
            chunk[packet_num][1],
            chunk[packet_num][2],
            chunk[packet_num][3]);
        chunk[packet_num][0] = coordinates[0];
        chunk[packet_num][1] = coordinates[1];
        chunk[packet_num][2] = coordinates[2];

        printf("        Data after swizzle -> X: %d, Y: %d, Z: %d\n", coordinates[0],
            coordinates[1],
            coordinates[2]);

        if (abs(previous_valid_packet[0] - coordinates[0]) > 25 && (packet_num != 0 || ignored_packets > 0)) {
            printf("        Ignoring packet. X: %d. Previous valid packet's X: %d. %d > 25.\n",
                coordinates[0],
                previous_valid_packet[0],
                abs(previous_valid_packet[0] - coordinates[0]));
            ignored_packets++;
            continue;
        }

        if (abs(previous_valid_packet[1] - coordinates[1]) > 25 && (packet_num != 0 || ignored_packets > 0)) {
            printf("        Ignoring packet. Y: %d. Previous valid packet's Y: %d. %d > 25.\n",
                coordinates[1],
                previous_valid_packet[1],
                abs(previous_valid_packet[1] - coordinates[1]));
            ignored_packets++;
            continue;

        }

        if (abs(previous_valid_packet[2] - coordinates[2]) > 25 && (packet_num != 0 || ignored_packets > 0)) {
            printf("        Ignoring packet. Z: %d. Previous valid packet's Z: %d. %d > 25.\n",
                coordinates[2],
                previous_valid_packet[2],
                abs(previous_valid_packet[2] - coordinates[2]));
            ignored_packets++;
            continue;
        }

        previous_valid_packet[0] = coordinates[0];
        previous_valid_packet[1] = coordinates[1];
        previous_valid_packet[2] = coordinates[2];

        valid_packets[packet_num - ignored_packets][0] = coordinates[0];
        valid_packets[packet_num - ignored_packets][1] = coordinates[1];
        valid_packets[packet_num - ignored_packets][2] = coordinates[2];
    }

    float sum_x = 0;
    float sum_y = 0;
    float sum_z = 0;
    for (int i = 0; i < packets_in_chunk - ignored_packets; i++) {
        sum_x += valid_packets[i][0];
        sum_y += valid_packets[i][1];
        sum_z += valid_packets[i][2];
    }

    float avg_x = sum_x / (packets_in_chunk - ignored_packets);
    float avg_y = sum_y / (packets_in_chunk - ignored_packets);
    float avg_z = sum_z / (packets_in_chunk - ignored_packets);

    printf("    Chunk Average X: %.2f, Average Y: %.2f, Average Z: %.2f\n\n", avg_x, avg_y, avg_z);

    return 0;
}

int main(int argc, char ** argv) {
    // Sample Command:
    // ./vr2017 ./binary_file.bin 0xFF 0xAA 0x11

    if (argc < 5) {
        printf("Error: Not enough command line arguments.\n");
        return -1;
    } else if (argc > 5) {
        printf("Error: Too many command line arguments.\n");
        return -1;
    }

    char * filename = argv[1];
    FILE * binary_file = fopen(argv[1], "rb");

    if (binary_file == NULL) {
        printf("Error: File %s does not exist!\n", filename);
        return 0;
    }

    for (int i = 2; i < argc; i++) {

        if (is_string_right_length(argv[i]) == -1) {
            printf("Error: Argument for delimiter byte %d is not of the correct length\n", i - 2);
            return -1;
        }
        
        if (is_string_start_with_0x(argv[i]) == -1) {
            printf("Error: Argument for delimiter byte %d does not begin with 0x\n", i - 2);
            return -1;
        }

        if (is_string_hex(argv[i]) == -1) {
            printf("Error: Argument for delimiter byte %d is not a valid hex value\n", i - 2);
            return -1;
        }
    }


    int byte0 = strtol(argv[2], NULL, 16);
    int byte1 = strtol(argv[3], NULL, 16);
    int byte2 = strtol(argv[4], NULL, 16);

    printf("Delimiter byte 0 is: %d\n", byte0);
    printf("Delimiter byte 1 is: %d\n", byte1);
    printf("Delimiter byte 2 is: %d\n", byte2);

    int checksum = bitwise_xor_of_three_numbers(byte0, byte1, byte2);
    printf("Checksum is: %d\n\n", checksum);

    int pointer_position = 0;
    int chunk[128][5];
    int packet_count = 0;
    int byte_count = 0;
    int new_chunk = 0;
    int byte;
    int chunk_num = 0;
    int offset = 0;
    int check_overflow = 0;

    while ((byte = getc(binary_file)) != EOF) {

        if (check_overflow == 1 && byte != byte0) {
            packet_count = 0;
            continue;
        }

        if (packet_count == 128) {
            packet_count++;
            chunk_processor(chunk, chunk_num, offset, packet_count, byte_count);
            check_overflow = 1;
            printf("\n");
            continue;
        }
        
        pointer_position++;

        if (new_chunk == 1) {
            chunk_num++;
            new_chunk = 0;
            for (int i = 0; i < 128; i++) {
                for (int j = 0; j < 5; j++) {
                    chunk[i][j] = 0;
                }
            }
            packet_count = 0;
            byte_count = 0;
        }

        if (byte == byte0) {
            int byte1_checker = fgetc(binary_file);
            pointer_position++;
            if (byte1_checker == byte1) {
                int byte2_checker = fgetc(binary_file);
                pointer_position++;
                if (byte2_checker == byte2) {
                    int checksum_checker = fgetc(binary_file);
                    pointer_position++;
                    if (checksum_checker == checksum) {
                        if (check_overflow == 0){
                            chunk_processor(chunk, chunk_num, offset, packet_count, byte_count);
                        } else {
                            check_overflow = 0;
                        }
                        offset = pointer_position;
                        new_chunk = 1;
                        continue;
                    } else {
                        fseek(binary_file, -3, SEEK_CUR);
                        pointer_position -= 3;
                    }
                } else {
                    fseek(binary_file, -2, SEEK_CUR);
                    pointer_position -= 2;
                }
            } else {
                fseek(binary_file, -1, SEEK_CUR);
                pointer_position -= 1;
            }
        }

        chunk[packet_count][byte_count] = byte;
        byte_count++;
        if (byte_count == 5) {
            byte_count = 0;
            packet_count++;
        }
    }
    if (new_chunk == 0 && pointer_position != 0) {
        chunk_processor(chunk, chunk_num, offset, packet_count, byte_count);
    }
    return 0;
}