#include <stdio.h>
#include<string.h>
#include<unistd.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#define RED "\x1B[31m"
#define GREEN "\x1B[32m"
#define YELLOW "\x1B[33m"
#define BLUE "\x1B[34m"
#define CYAN "\x1B[36m"
#define WHITE "\x1B[37m"
#define RESET "\x1B[0m"

int main(int argc, char *argv[]){
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;
    /*check operation type*/
    if(check_operation_type(argc,argv) == e_encode){
        printf(YELLOW "so ,Your are doing encoding\n"RESET);
        if(read_and_validate_encode_args(argc,argv,&encInfo) == e_failure){
           printf(RED "Failed Invalid arguments for encoding!"RESET"\n");
           return 1;
        }
        else{
            printf(GREEN"step2: passing Agruments validated successfully."RESET"\n");
            sleep(2);
        }
        if(do_encoding(&encInfo) == e_failure){
            printf(RED "Error: Encoding failed!"RESET"\n");
            return 1;
        }
        else{
           printf(BLUE "Encoding completed successfully."RESET"\n");
           sleep(1);
        }
    }
    else if(check_operation_type(argc,argv) == e_decode){
        printf(CYAN "so,You are doing Decoding." RESET"\n");
        sleep(1);
        if(read_and_validate_decode_args(argc,argv,&decInfo) == e_failure){
           printf(RED "Failed Invalid arguments for decoding."RESET"\n");
           return 1;
        }
        else{
           printf(GREEN "passing Agruments validated successfully."RESET"\n");
           sleep(2);
        }
        if(do_decoding(&decInfo) == e_failure)
        {
           printf(RED "Error: Decoding failed"RESET"\n");
           return 1;
        }
        else{
           printf(BLUE "Decoding completed successfully."RESET"\n");
           sleep(1);
        }
    }
    else{
        printf(RED "Unsupported operation."RESET"\n");
    }
    return 0;
}
OperationType check_operation_type(int argc, char *argv[])
{    
    if(strcmp(argv[1],"-e") == 0)//check encoding operation or not
    {
        return e_encode;
    }
    else if(strcmp(argv[1],"-d")==0)//check decoding operation or not
    {
        return e_decode;
    }
    else// unsupport operation 
    {
        return e_unsupported;
    }
}

    
    

