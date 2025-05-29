#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define RESET "\x1B[0m"


Status decode_open_files(DecodeInfo *decInfo)
{
    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");

    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED "ERROR: Unable to open file %s "RESET"\n", decInfo->stego_image_fname);
        return e_failure;
    }

    return e_success;
}

Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
    if (argc < 3)
    {
        printf(RED "ERROR: in passing arguments  "RESET"\n");
        return e_failure;
    }

    if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        decInfo->stego_image_fname = argv[2];
    }
    else
    {
        printf(RED "ERROR: Source image must be '.bmp' file."RESET"\n");
        return e_failure;
    }

    // Check if an output file is provided (argv[3])
    if (argc > 3 && argv[3] != NULL)
    {
        char *token = strtok(argv[3], ".");
        if (token != NULL && strlen(token) < MAX_FILENAME_SIZE)
        {
            strcpy(decInfo->output_data_fname, token);
        }
        else
        {
            printf(RED  "ERROR: Invalid output file format."RESET"\n");
            return e_failure;
        }
    }
    else
    {
        // Set default filename
        strcpy(decInfo->output_data_fname, "output");
    }

    return e_success;
}

Status do_decoding(DecodeInfo *decInfo)
{
    printf("Enter your Decoding Password: ");
    scanf("%19s", decInfo->password);
    sleep(2);
    if (decode_open_files(decInfo) == e_failure)
        return e_failure;
    printf(YELLOW "step1: Skipping bmp header in progress"RESET"\n");
    sleep(1);
    if (skip_bmp_header(decInfo) == e_failure)
        return e_failure;
    printf(YELLOW "step2: Decoding magic string in progress"RESET"\n");
    sleep(1);
    if (decode_magic_string(decInfo) == e_failure)
    {
        printf(RED "ERROR: your Decoding failed  Password mismatch ."RESET"\n");
        sleep(1);
        return e_failure;
    }
        printf(YELLOW "step3: Decoding secret file extension size "RESET"\n");
        sleep(1);
    if (decode_secret_file_extn_size(decInfo->fptr_stego_image, decInfo) == e_failure)
        return e_failure;
        printf(YELLOW "step4: Decoding secret file extension "RESET"\n");
        sleep(1);
    if (decode_secret_file_extn(decInfo) == e_failure)
        return e_failure;
        printf(YELLOW "step5: Decoding secret data size"RESET"\n");
        sleep(1);
       int secret_size_res =  decode_secret_data_size(decInfo->fptr_stego_image,DATA_SIZE,decInfo);
    if(secret_size_res == e_failure)  
    { 
        printf(RED "ERROR: Decoding secret data size failed."RESET"\n");
        sleep(1);
        return e_failure;   
    }
    if(secret_size_res==e_success)
    {
        printf(GREEN "step6: Secret Data Size decoded Succesfully!"RESET"\n");
        sleep(1);          
    }
    decInfo->secret_data = (char*)malloc(decInfo->secret_data_size);
    if(decInfo->secret_data == NULL)
    {
        printf(RED "ERROR: Memory allocation failed for secret data."RESET"\n");
        return e_failure;
    }
        printf(YELLOW "step7: Decoding secret data in progress"RESET"\n");
        sleep(1);
    int secret_res=decode_image_to_data(decInfo->fptr_stego_image,decInfo->secret_data,decInfo->secret_data_size,decInfo);
    if(secret_res==e_failure)
    {
        printf(RED "ERROR : decoding Data failed."RESET"\n");
        sleep(1);
        free(decInfo->secret_data);// free allocated memory
        return e_failure;          
    }
    if(secret_res==e_success)
    {
        printf(GREEN "step8: decoded Succesfully!"RESET"\n");
        sleep(1);          
    }
    FILE* fptr=fopen(decInfo->output_data_fname,"w");
    if(fptr == NULL)
    {
        printf(RED "ERROR: not to open output file."RESET"\n");
        free(decInfo->secret_data);
        return e_failure;
    }
    fwrite(decInfo->secret_data,decInfo->secret_data_size,1,fptr);
    fclose(fptr);
    free(decInfo->secret_data);

    printf(GREEN "Decoding completed successfully."RESET"\n");
    return e_success;
}

Status skip_bmp_header(DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);
    printf(GREEN "Skipped BMP header successfully."RESET"\n");
    sleep(2);
    return e_success;
}

Status decode_magic_string(DecodeInfo *decInfo)
{
    if (decode_image_to_data(decInfo->fptr_stego_image, decInfo->magic_string, MAGIC_SIZE, decInfo) == e_failure)
        return e_failure;

    if (strcmp(decInfo->magic_string, decInfo->password) == 0)
    {
        printf(GREEN "Password matched successfully."RESET"\n");
        sleep(2);
        return e_success;
    }
    else
    {
        printf(RED "ERROR: Password mismatch."RESET"\n");
        sleep(1);
        return e_failure;
    }
}

Status decode_image_to_data(FILE *fptr_stego, char *str, int str_size, DecodeInfo *decInfo)
{
    for (int i = 0; i < str_size; i++)
    {
        char buffer[8];
        fread(buffer, sizeof(char), 8, fptr_stego);

        if (decode_byte_from_lsb(buffer, &str[i]) == e_failure)
        {
            printf(RED "ERROR: Decoding byte from lsb is failed."RESET"\n");
            sleep(2);
            return e_failure;
        }
    }
    str[str_size] = '\0';
    printf(GREEN "Decoding image to data is successfull."RESET"\n");
    sleep(2);
    return e_success;
}

Status decode_byte_from_lsb(char *buffer, char *ch)
{
    *ch = 0;
    for (int i = 0; i < 8; i++)
    {
        *ch = (*ch << 1) | (buffer[i] & 1);
    }
    return e_success;
}

Status decode_secret_file_extn_size(FILE *fptr_stego, DecodeInfo *decInfo)
{
    char buffer[EXTN_SIZE];
    fread(buffer, sizeof(char), EXTN_SIZE, fptr_stego);

    if (decode_size_from_lsb(EXTN_SIZE, buffer, decInfo) == e_failure)
    {
        printf(RED "ERROR: Decoding size from lsb failed."RESET"\n");
        sleep(2);
        return e_failure;
    }

    printf(GREEN "Decoding secret file extension size successfull."RESET"\n");
    sleep(2);
    return e_success;
}

Status decode_size_from_lsb(int size, char *buffer, DecodeInfo *decInfo)
{
    int decoded_size = 0;

    for (int i = 0; i < size; i++)
    {
        decoded_size = (decoded_size << 1) | (buffer[i] & 1);
    }

    decInfo->secret_file_extn_size = decoded_size;
    return e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char buffer[FILE_EXTN_SIZE + 1];
    if (decode_image_to_data(decInfo->fptr_stego_image, buffer, FILE_EXTN_SIZE, decInfo) == e_failure)
        return e_failure;

    buffer[FILE_EXTN_SIZE] = '\0';
    printf(GREEN "Secret File extension Decoded successfully: %s"RESET"\n", buffer);
    strcat(decInfo->output_data_fname,buffer);
    sleep(2);
    return e_success;
}

Status decode_secret_data_size(FILE* fptr_stego, int size, DecodeInfo *decinfo)
{
    char data_buffer[size];
    int ret;
    fread(data_buffer,size,1,fptr_stego);
    ret = decode_size_from_lsb_data(data_buffer,decinfo);
    return e_success;

}

Status decode_size_from_lsb_data(char* data_buffer, DecodeInfo *decinfo)
{
    int k=0;
    int n=31;    
    for(int i=0;i<DATA_SIZE;i++)
    {
        k = ((data_buffer[i]&1)<<n) | k;
        n--;
    }
    decinfo->secret_data_size=k;
    return e_success;
}

