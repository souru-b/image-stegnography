#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */
#define MAX_SECRET_BUF_SIZE 100
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _EncodeInfo
{   
    /* Source Image info */
    char *src_image_fname;/* Store the src_image_name*/
    FILE *fptr_src_image;/*Store the src_image*/
    uint image_capacity;/*store the size of your image*/
    uint bits_per_pixel;
    char image_data[MAX_IMAGE_BUF_SIZE];/*to store the data*/

    /* Secret File Info */
    char *secret_fname;/*to store the secret file name*/
    FILE *fptr_secret;/*to store the secret  file pointer*/
    char extn_secret_file[MAX_FILE_SUFFIX];/*to store the extension of given secret  file*/
    int extn_secret_file_size;
    char secret_data[MAX_SECRET_BUF_SIZE];/*to store the  data in the secret file*/
    int size_secret_file;/*to store the size of the secret file*/

    /* Stego Image Info */ /* output.bmp info*/
    char *stego_image_fname;/*to store the output file name*/
    FILE *fptr_stego_image;/*to store the output file pointer*/

} EncodeInfo;



/* Check operation type */
OperationType check_operation_type(int argc, char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo);

/*Encode secret fileextension size*/

Status encode_secret_file_extn_size(int extn_secret_file_size, EncodeInfo *encinfo);

/* Encode secret file size */
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Encode size to lsb*/
Status encode_size_to_lsb(int size, char *buffer,EncodeInfo *encinfo);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif
