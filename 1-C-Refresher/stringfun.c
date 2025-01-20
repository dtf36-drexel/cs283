#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
//add additional prototypes here
void normalize_spaces(char *, int);
void reverse_string(char *, int);
void uppercase_string(char *, int);

int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    char *start = buff;
    int count = 0;
    int in_space = 0;

    while (*user_str == ' ' || *user_str == '\t') {
        user_str++;
    }

    while (*user_str && count < len) {
        if (*user_str != ' ' && *user_str != '\t') {
            *buff++ = *user_str;
            in_space = 0;
            count++;
        } else if (!in_space) {
            *buff++ = ' ';
            in_space = 1;
            count++;
        }
        user_str++;
    }

    if (*user_str != '\0') {
        printf("Error: Provided input string is too long\n");
        return -1;
    }

    if (*(buff - 1) == ' ') {
        buff--;
        count--;
    }

    while (count < len) {
        *buff++ = '.';
        count++;
    }

    return (buff - start);
}

void print_buff(char *buff, int len){
    printf("Buffer:  [");
    for (int i = 0; i < len && *(buff + i) != '\0'; i++) {
        putchar(*(buff+i));
    }
    putchar(']');
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

void normalize_spaces(char *buff, int len) {
    int foo = 0;
    int bar = 0;
    int in_space = 0;

    while (bar < len && *(buff + bar) != '\0') {
        if (*(buff + bar) != ' ') {
            *(buff + foo++) = *(buff + bar);
            in_space = 0;
        } else if (!in_space) {
            *(buff + foo++) = ' ';
            in_space = 1;
        }
        bar++;
    }
    if (foo > 0 && *(buff + foo - 1) == ' ') {
        foo--;
    }
    *(buff + foo) = '\0';
}

int count_words(char *buff, int len, int str_len){
    normalize_spaces(buff, str_len);
    int count = 0;
    int foo = 0;
    for (int i = 0; i < str_len; i++) {
        if (*(buff + i) != ' ' && *(buff + i) != '\0') {
            if (!foo) {
                foo = 1;
                count++;
            }
        } else {
            foo = 0;
        }
        if(i >= len){
            break;
        }
    }
    return count;
}


//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS

void word_print(char *buff, int len) {
    printf("Word Print\n----------\n");
    int word_count = 0;
    int char_count = 0;
    char *start = buff;

    for (int i = 0; i < len && buff[i] != '\0' && buff[i] != '.'; i++) {
        if (buff[i] != ' ') {
            char_count++;
        } else if (char_count > 0) {
            printf("%d. %.*s(%d)\n", ++word_count, char_count, start, char_count);
            char_count = 0;
            start = buff + i + 1;
        }
    }

    if (char_count > 0) {
        printf("%d. %.*s(%d)\n", ++word_count, char_count, start, char_count);
    }

    printf("\nNumber of words returned: %d\n", word_count);
}

void reverse_string(char *buff, int len) {
    int i = 0;
    while (i < len && buff[i] != '\0' && buff[i] != '.') {
        i++;
    }

    int j = i - 1;
    i = 0;
    while (i < j) {
        char temp = buff[i];
        buff[i] = buff[j];
        buff[j] = temp;
        i++;
        j--;
    }
}

void uppercase_string(char *buff, int len) {
    for (int i = 0; i < len && *(buff + i) != '\0'; i++) {
        if (*(buff + i) >= 'a' && *(buff + i) <= 'z') {
            *(buff + i) -= 32;
        }
    }
}

void search_and_replace() {
    printf("Not Implemented!\n");
    exit(3);
}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    /*
    By checking if argc < 2 we ensure that argv[1] already exists.
    If there are fewer than 2 arguments, the program will automatically terminate.
    */
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    /*
    This checks if there are at least 3 arguments.
    It ensures that there is a string input provided.
    */
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = (char*)malloc(BUFFER_SZ);
    if(!buff) {
        fprintf(stderr, "Error: Memory Allocation Failure.\n");
        exit(99);
    }
    memset(buff,0,BUFFER_SZ);

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            print_buff(buff, BUFFER_SZ);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options

        case 'r':
            reverse_string(buff, BUFFER_SZ);
            print_buff(buff, BUFFER_SZ);
            break;

        case 'w':
            word_print(buff, BUFFER_SZ);
            print_buff(buff, BUFFER_SZ);
            break;
        case 'x':
            if (argc != 5) {
                printf("Error: Insufficient arguments for -x\n");
                exit(1);
            }
            search_and_replace();
            break;

        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE

//          Redundancy. It can help avoid overflows.
