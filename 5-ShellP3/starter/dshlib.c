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

void handle_redirection(cmd_buff_t *cmd) {
    int i = 0;
    while (cmd->argv[i] != NULL) {
        if (strcmp(cmd->argv[i], "<") == 0) {
            if (cmd->argv[i+1] == NULL) {
                fprintf(stderr, "No input file specified\n");
                exit(EXIT_FAILURE);
            }
            int fd = open(cmd->argv[i+1], O_RDONLY);
            if (fd < 0) {
                perror("Input file error");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDIN_FILENO);
            close(fd);
            // Shift left by 2: remove the redirection token and its filename
            int j = i;
            while (cmd->argv[j+2] != NULL) {
                cmd->argv[j] = cmd->argv[j+2];
                j++;
            }
            cmd->argv[j] = NULL;
            cmd->argv[j+1] = NULL;
            continue;  // recheck current index in case there are multiple redirections
        }
        else if (strcmp(cmd->argv[i], ">") == 0) {
            if (cmd->argv[i+1] == NULL) {
                fprintf(stderr, "No output file specified\n");
                exit(EXIT_FAILURE);
            }
            int fd = open(cmd->argv[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                perror("Output file error");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            int j = i;
            while (cmd->argv[j+2] != NULL) {
                cmd->argv[j] = cmd->argv[j+2];
                j++;
            }
            cmd->argv[j] = NULL;
            cmd->argv[j+1] = NULL;
            continue;
        }
        else if (strcmp(cmd->argv[i], ">>") == 0) {
            if (cmd->argv[i+1] == NULL) {
                fprintf(stderr, "No output file specified for append\n");
                exit(EXIT_FAILURE);
            }
            int fd = open(cmd->argv[i+1], O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0) {
                perror("Append file error");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
            int j = i;
            while (cmd->argv[j+2] != NULL) {
                cmd->argv[j] = cmd->argv[j+2];
                j++;
            }
            cmd->argv[j] = NULL;
            cmd->argv[j+1] = NULL;
            continue;
        }
        i++;
    }
}


int execute_pipeline(command_list_t *clist)
{
    int num_cmds = clist->num;
    int pipes[CMD_MAX - 1][2];  // Pipes for communication
    pid_t pids[CMD_MAX];

    // Create pipes
    for (int i = 0; i < num_cmds - 1; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe error");
            return ERR_EXEC_CMD;
        }
    }

    // Fork child processes
    for (int i = 0; i < num_cmds; i++) {
        pids[i] = fork();

        if (pids[i] < 0) {
            perror("fork error");
            return ERR_EXEC_CMD;
        }

        if (pids[i] == 0) { // Child process
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i < num_cmds - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            // Close pipes in child process
            for (int j = 0; j < num_cmds - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            handle_redirection(&clist->commands[i]);

            // Execute command
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp error");
            exit(EXIT_FAILURE);
        }
    }

    // Close pipes in parent process
    for (int i = 0; i < num_cmds - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for children
    for (int i = 0; i < num_cmds; i++) {
        waitpid(pids[i], NULL, 0);
    }

    return OK;
}



int build_cmd_list(char *cmd_line, command_list_t *clist)
{
    if (!cmd_line || strlen(cmd_line) == 0) {
        return WARN_NO_CMDS;
    }

    memset(clist, 0, sizeof(command_list_t));
    char *token = strtok(cmd_line, PIPE_STRING);
    int cmdCount = 0;

    while (token != NULL) {
        if (cmdCount >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

        // Trim spaces
        while (*token == SPACE_CHAR) token++;
        char *end = token + strlen(token) - 1;
        while (end > token && *end == SPACE_CHAR) *end-- = '\0';

        // Initialize cmd_buff_t
        if (alloc_cmd_buff(&clist->commands[cmdCount]) != OK) {
            return ERR_MEMORY;
        }
        clear_cmd_buff(&clist->commands[cmdCount]);

        // Parse command and arguments
        build_cmd_buff(token, &clist->commands[cmdCount]);

        clist->num++;
        cmdCount++;

        token = strtok(NULL, PIPE_STRING);
    }

    return OK;
}



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
    else {
         return BI_NOT_BI;
    }
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
        handle_redirection(cmd);
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
    if (!cmd_buff) {
        fprintf(stderr, "Memory allocation failed for cmd_buff\n");
        return ERR_MEMORY;
    }

    if (alloc_cmd_buff(&cmd) != OK) {
        fprintf(stderr, "Memory allocation failed for command buffer\n");
        free(cmd_buff);
        return ERR_MEMORY;
    }

    while (1) {
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL) {
            printf("\n");
            break;
        }
        cmd_buff[strcspn(cmd_buff, "\n")] = '\0';  // Remove the trailing newline

        if (strlen(cmd_buff) == 0) {
            printf(CMD_WARN_NO_CMD);
            continue;
        }

        // Check for pipe character
        if (strchr(cmd_buff, PIPE_CHAR) != NULL) {
            command_list_t clist;
            memset(&clist, 0, sizeof(command_list_t));

            rc = build_cmd_list(cmd_buff, &clist);
            if (rc != OK) {
                if (rc == ERR_TOO_MANY_COMMANDS) {
                    printf(CMD_ERR_PIPE_LIMIT, CMD_MAX);
                } else {
                    fprintf(stderr, "Error parsing pipeline command\n");
                }
                
                for (int i = 0; i < clist.num; i++) {
                    free_cmd_buff(&clist.commands[i]);
                }
                continue;
            }

            rc = execute_pipeline(&clist);
            if (rc != OK) {
                fprintf(stderr, "Pipeline execution error: %d\n", rc);
            }

            for (int i = 0; i < clist.num; i++) {
                free_cmd_buff(&clist.commands[i]);
            }
            continue;
        }

        clear_cmd_buff(&cmd);
        rc = build_cmd_buff(cmd_buff, &cmd);
        if (rc == WARN_NO_CMDS) {
            printf(CMD_WARN_NO_CMD);
            continue;
        } else if (rc != OK) {
            fprintf(stderr, "Error parsing command\n");
            continue;
        }

        if (match_command(cmd.argv[0]) != BI_NOT_BI) {
            exec_built_in_cmd(&cmd);
        } else {
            int exec_status = exec_cmd(&cmd);
            if (exec_status != OK) {
                fprintf(stderr, "execvp error: %d\n", exec_status);
            }
        }

        for (int i = 0; i < cmd.argc; i++) {
            free(cmd.argv[i]);
        }
    }

    free_cmd_buff(&cmd);
    free(cmd_buff);
    return OK;
}
