#include <stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#define BLACK "\x1B[30m"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define MAGENTA "\x1B[35m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"
#define RESET "\x1B[0m"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

Status check_capacity(EncodeInfo *encInfo)
{
    uint size_of_src = get_image_size_for_bmp(encInfo->fptr_src_image);
    fseek(encInfo->fptr_secret,0,SEEK_END);
    encInfo->size_secret_file= ftell(encInfo->fptr_secret);
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    uint image_capacity = 54 + (strlen(MAGIC_STRING) * 8) + 32 + (4 * 8) + 32 + (encInfo->size_secret_file * 8);
    if(image_capacity > size_of_src)
    {
        
        return e_failure;
    }
     printf(GREEN "step5: Capacity check successful."RESET"\n");
     sleep(1);
     return e_success;
}
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, RED "ERROR: Unable to open file %s"RESET"\n", encInfo->src_image_fname);

    	return e_failure;
    }

     printf(GREEN "step2: Source image file opened successfully."RESET"\n");
     sleep(1);

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    // Do Error handling
    
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, RED "ERROR: Unable to open file %s"RESET"\n", encInfo->secret_fname);

    	return e_failure;
    }
    printf(GREEN "step3: Secret file opened successfully."RESET"\n");
    sleep(1);

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, RED "ERROR: Unable to open file %s"RESET"\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    printf(GREEN "step4: Stego image file opened successfully."RESET"\n");
    sleep(1);
    return e_success;
}

Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
    {
            if (argc < 4)
            {
                printf(RED "Error: passing arguments."RESET"\n");
                return e_failure;
            }
            if(strcmp(strstr(argv[2],"."),".bmp") == 0)
            {
                encInfo->src_image_fname = argv[2];                 
            }
            else
            {
                printf(RED "Error: Source image must be  '.bmp' file."RESET"\n");
                return e_failure;
            }

            if(strcmp(strstr(argv[3],"."),".txt") == 0)
            {
                encInfo->secret_fname = argv[3];
                strcpy(encInfo->extn_secret_file,".txt");
               
            }
            else if(strcmp(strstr(argv[3],"."),".c") == 0)
            {
                encInfo->secret_fname = argv[3];
                strcpy(encInfo->extn_secret_file,".c");
               
            }
            else if(strcmp(strstr(argv[3],"."),".sh") == 0)
            {
                encInfo->secret_fname = argv[3];
                strcpy(encInfo->extn_secret_file,".sh");
               
            }
            else
            {
                printf(RED "Error: Secret file must be .txt, .c or .sh"RESET"\n");
                return e_failure;
            }

            if (argc >= 5 && argv[4] != NULL)
            {
                if (strstr(argv[4], ".") != NULL && strcmp(strstr(argv[4], "."), ".bmp") == 0)
                {
                    encInfo->stego_image_fname = argv[4];
                }
                else
                {
                    printf(RED "Error: Output image must be a '.bmp' file."RESET"\n");
                    return e_failure;
                }
            }
            else
            {
                // Assign default output file
                encInfo->stego_image_fname = "stego.bmp";

                // Open the default file in write mode
                FILE *fp = fopen(encInfo->stego_image_fname, "wb");
                if (fp == NULL)
                {
                    printf(RED "Error: not to create default output file 'stego.bmp'."RESET"\n");
                    return e_failure;
                }
                else
                {
                    printf(GREEN "step1: output file 'stego.bmp' opened successfully"RESET"\n"); // Debug
                    fclose(fp); //close the file 
                }
            }

    return e_success;

    }


Status do_encoding(EncodeInfo *encInfo)
{
    
    if(open_files(encInfo) == e_failure)
    {
        printf(GREEN "Error in opening files "RESET"\n");
        return e_failure;
    }
    else if(check_capacity(encInfo) == e_failure)
    {
        printf(GREEN "Error in check capacity"RESET"\n");
        return e_failure;
    }
    else if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)== e_failure)
    {
        printf(GREEN "Error in copy bmp header"RESET"\n");
        return e_failure;
    }
    else if(encode_magic_string(MAGIC_STRING,encInfo) == e_failure)
    {
        printf(GREEN "Error in encode magic string"RESET"\n");
        return e_failure;
    }
    else if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_failure)
    {
         printf(GREEN "Error in secret file extension size"RESET"\n");
        return e_failure;
    }
    else if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
         printf(GREEN "Error in secret file extension "RESET"\n");
        return e_failure;

    }
    else if(encode_secret_file_size((encInfo->size_secret_file), encInfo) == e_failure)
    {
        printf(GREEN "Error in secret file size "RESET"\n");
        return e_failure;

    }
    else if(encode_secret_file_data(encInfo) == e_failure)
    {
         printf(GREEN "Error in secret file data "RESET"\n");
        return e_failure;

    }
    else if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure)
    {
        printf( GREEN "Error in copying remaining image data to stego image."RESET"\n");
        return e_failure;
    }

    printf(GREEN "Encoding process completed successfully."RESET"\n");
    sleep(1);
    return e_success; 
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char buffer[54];

    rewind(fptr_src_image);
    if(fread(buffer,54,1,fptr_src_image) != 1)
    {
        printf(RED "Error reading BMP header."RESET"\n");
        return e_failure;
    }
    if(fwrite(buffer,54,1,fptr_dest_image) != 1)
    {
        printf(RED "Error writing BMP header."RESET"\n");
        return e_failure;
    }
    printf(GREEN "step6: BMP header copied successfully."RESET"\n");
    sleep(1);
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    

    if(encode_data_to_image(MAGIC_STRING,strlen(MAGIC_STRING),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
        printf(GREEN "step7: Magic string encoded successfully."RESET"\n");
        sleep(1);
        return e_success;
    }

    
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];
    for(int i=0; i < size; i++)
    {
        size_t bytes_read = fread(buffer,8,1,fptr_src_image);
        if(bytes_read !=1)    
        {
            printf(RED "Error: fread reads only %zu bytes "RESET"\n",bytes_read);
            return e_failure;
        }
        if(encode_byte_to_lsb(data[i],buffer) == e_failure)
        {
            printf(GREEN "Error in encode byte to lsb"RESET"\n");
            return e_failure;
        }

            size_t bytes_written = fwrite(buffer,8,1,fptr_stego_image);
        if(bytes_written !=1) 
        {
            printf(RED "Error: fread wrote only %zu bytes"RESET"\n",bytes_written);
            return e_failure;
        }  
       
    }
    printf(YELLOW "Data encoded successfully to image."RESET"\n");
    sleep(1);
    return e_success;
}

Status encode_byte_to_lsb(char data, char* buffer)
{
    
    for(int i=0;i<8;i++)
    {
        buffer[i] = ((data>>(7-i))&1) | (buffer[i] & (~1));
       
    }

    return e_success;
}  
    
Status encode_secret_file_extn_size(int size, EncodeInfo *encinfo)
{
    char buffer[32];/* size to lsb function */
    
    fread(buffer,32,1,encinfo->fptr_src_image);
   
    for(int i=0;i<31;i++)
    {
        buffer[i] = ((size>>(31-i))&1) | (buffer[i] & (~1));
           
    }

    if(fwrite(buffer,32,1,encinfo->fptr_stego_image) < 1)
    {
        printf(RED "Error failed to write the size to destination file."RESET"\n");
        return e_failure;
    }
    else
    {
        printf(GREEN "step8: size encoded successfully to LSB."RESET"\n");
        sleep(1);
        return e_success;
    }    
    
}

Status encode_secret_file_extn(char* extn_file, EncodeInfo *encInfo)
{
    if(encode_data_to_image(extn_file,strlen(extn_file),encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_failure)
    {
        printf(RED "Error secret file extension encode is failed."RESET"\n");
        return e_failure;
    }

    return e_success;

}

Status encode_secret_file_size(int filesize, EncodeInfo *encinfo)
{

    /* size to lsb to encode secret file size*/
    char buffer[32];
    fread(buffer,32,1,encinfo->fptr_src_image);

    for(int i=0;i<31;i++)
    {
        buffer[i] = ((filesize>>(31-i))&1) | (buffer[i] & (~1));
               
    }
    if(fwrite(buffer,32,1,encinfo->fptr_stego_image) < 1)
    {
        printf(RED "Error failed to write the size to destination file."RESET"\n");
        return e_failure;
    }
    else
    {
        printf(GREEN "step9: secret file size encoded successfully to LSB."RESET"\n");
        sleep(1);
        return e_success;
    }
}

Status encode_secret_file_data(EncodeInfo *encinfo)
{
    char ch;
    int i=0;
    int size = encinfo->size_secret_file;
    char secret_data[size+1];
    while ((ch=fgetc(encinfo->fptr_secret)) != EOF)
    {
        secret_data[i++] = ch;
    }

    secret_data[i] = '\0';
    
    if(encode_data_to_image(secret_data,encinfo->size_secret_file,encinfo->fptr_src_image,encinfo->fptr_stego_image) == e_failure)
    {
        printf(RED "Error in encoding secret file data failed."RESET"\n");
        return e_failure;
    }
    return e_success;

}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    size_t bytes_read;

    while((bytes_read = fread(&ch,1,1,fptr_src)) > 0)
    {
        if(fwrite(&ch,1,1,fptr_dest) != bytes_read)
        {
            printf(RED "Error in writing remaining data to destination file."RESET"\n");
            return e_failure;
        }
    }
    
    printf(GREEN "step10: Remaining image data copied successfully."RESET"\n");
    sleep(1);
    return e_success;

}



