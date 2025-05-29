#ifndef DECODE_H
#define DECODE_H

#include<stdio.h>
#include "types.h"

#define DATA_SIZE 32
#define EXTN_SIZE 32
#define MAGIC_SIZE 2
#define SIZE 8
#define FILE_EXTN_SIZE 4
#define MAX_FILENAME_SIZE 255

typedef struct _DecodeInfo
{
    char *stego_image_fname;
    FILE *fptr_stego_image;

    char output_data_fname[MAX_FILENAME_SIZE];
    FILE *fptr_output_fname;

    char magic_string[MAGIC_SIZE + 1];
    char password[20];
    int secret_file_extn_size;
    int secret_data_size;
    char *secret_data;
    

} DecodeInfo;

Status do_decoding(DecodeInfo *decInfo);
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo);
Status decode_open_files(DecodeInfo *decInfo);
Status skip_bmp_header(DecodeInfo *decInfo);
Status decode_magic_string(DecodeInfo *decInfo);
Status decode_image_to_data(FILE *fptr_stego, char *string, int string_size, DecodeInfo *decInfo);
Status decode_byte_from_lsb(char *data, char *ch);
Status decode_secret_file_extn(DecodeInfo *decInfo);
Status decode_secret_file_extn_size(FILE *fptr_stego_image, DecodeInfo *decInfo);
Status decode_secret_data_size(FILE *fptr_stego, int size, DecodeInfo *decInfo);
Status decode_size_from_lsb(int size, char *buffer, DecodeInfo *decInfo);
Status decode_size_from_lsb_data(char *data_buffer, DecodeInfo *decInfo);

#endif
