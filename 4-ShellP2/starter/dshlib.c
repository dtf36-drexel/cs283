#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

extern void print_dragon();


int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = (char *)malloc(SH_CMD_MAX);
    if (!cmd_buff->_cmd_buffer) {
        return ERR_MEMORY;
    }
    cmd_buff->argc = 0;
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer) {
        free(cmd_buff->_cmd_buffer);
    }
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->argc = 0;
    for (int i = 0; i < CMD_ARGV_MAX; i++) {
        cmd_buff->argv[i] = NULL;
    }
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    if (!cmd_line || strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }

    strncpy(cmd_buff->_cmd_buffer, cmd_line, SH_CMD_MAX - 1);
    cmd_buff->_cmd_buffer[SH_CMD_MAX - 1] = '\0';
    
    char *input = cmd_buff->_cmd_buffer;
    
    //Remove leading spaces
    while (*input == SPACE_CHAR) input++;
    char *end = input + strlen(input) - 1;
    while (end > input && *end == SPACE_CHAR) *end-- = '\0';
    //remove trailing space
    
    cmd_buff->argc = 0;
    char *ptr = input;
    while(*ptr != '\0'){
        if(*ptr == '\0') break;

        char token[ARG_MAX];
        int index = 0;

        if(*ptr =='\"'){
            ptr++; //skip quote
            while(*ptr && *ptr != '\"' && index < ARG_MAX - 1){
                token[index++] = *ptr;
                ptr++;
            }
            token[index] = '\0';
            if(*ptr == '\"') ptr++; // skip quote
        } else{
            while(*ptr && (*ptr != SPACE_CHAR) && index < ARG_MAX - 1){
                token[index++] = *ptr;
                ptr++;
            }
            token[index] = '\0';
            while (*ptr && (*ptr == SPACE_CHAR)) {
                ptr++;
            }
        }
        cmd_buff->argv[cmd_buff->argc] = strdup(token);
        if (!cmd_buff->argv[cmd_buff->argc])
            return ERR_MEMORY;
        cmd_buff->argc++;
        
        if (cmd_buff->argc >= CMD_ARGV_MAX - 1)
            break;
    }
    cmd_buff->argv[cmd_buff->argc] = NULL;
    return OK;
}

Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, "exit") == 0)
         return BI_CMD_EXIT;
    else if (strcmp(input, "dragon") == 0)
         return BI_CMD_DRAGON;
    else if (strcmp(input, "cd") == 0)
         return BI_CMD_CD;
    else if (strcmp(input, "rc") == 0)
         return BI_RC;
    else 
         return BI_NOT_BI;
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds bi = match_command(cmd->argv[0]);
    if (bi == BI_CMD_EXIT) {
         exit(0);
    } else if (bi == BI_CMD_DRAGON) {
         print_dragon();
         return BI_EXECUTED;
    } else if (bi == BI_CMD_CD) {
         if (cmd->argc == 1) {
             /* Do nothing if no argument is provided */
         } else if (cmd->argc == 2) {
             if (chdir(cmd->argv[1]) != 0) {
                 perror("cd error");
             }
         } else {
             fprintf(stderr, "cd: too many arguments\n");
         }
         return BI_EXECUTED;
    }
    return BI_NOT_BI;
}

int exec_cmd(cmd_buff_t *cmd) {
    int f_result = fork();
    int c_result;

    if (f_result < 0) {
        perror("fork failed");
        return ERR_EXEC_CMD;
    }
    if (f_result == 0) {
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp error");
        exit(EXIT_FAILURE);
    } else {
        wait(&c_result);
        return WEXITSTATUS(c_result);
    }
}


int exec_local_cmd_loop()
{
    char *cmd_buff;
    int rc = 0;
    cmd_buff_t cmd;

    cmd_buff = malloc(ARG_MAX * sizeof(char));
    if (alloc_cmd_buff(&cmd) != OK) {
        fprintf(stderr, "Memory allocation failed\n");
        return ERR_MEMORY;
    }


    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';  // Remove the trailing newline

        // IMPLEMENT parsing input to cmd_buff_t *cmd_buff
        clear_cmd_buff(&cmd);  // Reset previous tokens
        rc = build_cmd_buff(cmd_buff, &cmd);
        if (rc == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
            continue;
        } else if (rc != OK) {
            fprintf(stderr, "Error parsing command\n");
            continue;
        }

        // IMPLEMENT if built-in command, execute builtin logic for exit, cd, dragon, rc
        if (match_command(cmd.argv[0]) != BI_NOT_BI) {
            exec_built_in_cmd(&cmd);
            for (int i = 0; i < cmd.argc; i++) {
                free(cmd.argv[i]);
            }
            continue;
        }

        if (match_command(cmd.argv[0]) != BI_NOT_BI) {
            exec_built_in_cmd(&cmd);
        } else {
            int exec_status = exec_cmd(&cmd);
            if (exec_status != OK) {
                fprintf(stderr, "execvp error: %d", exec_status);
            }
        }

        for (int i = 0; i < cmd.argc; i++) {
            free(cmd.argv[i]);
        }
    }

    free_cmd_buff(&cmd);
    return OK;
}

