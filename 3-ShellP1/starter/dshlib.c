#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first input as the executable name, and
 *  then the remaining inputs as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */
int build_cmd_list(char *cmd_line, command_list_t *clist)
{   
    //Check for empty command
    if (cmd_line == NULL || strlen(cmd_line) == 0)
    {
        return WARN_NO_CMDS;
    }

    memset(clist, 0, sizeof(command_list_t));
    char *input = strtok(cmd_line, PIPE_STRING); //Parse by splitting based on pipes
    int cmdCount = 0;

    while (input != NULL)
    {
        if (cmdCount >= CMD_MAX)
        {
            return ERR_TOO_MANY_COMMANDS;
        }
        
        //Remove leading spaces
        while (*input == SPACE_CHAR) input++;
        char *end = input + strlen(input) - 1;
        while (end > input && *end == SPACE_CHAR) *end-- = '\0';
        //remove trailing space

        //Check for command too large
        if (strlen(input) >= EXE_MAX)
        {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }
        
        char *argStart = strchr(input, SPACE_CHAR);
        if (argStart)
        {
            *argStart = '\0';
            //Check for args too big
            if (strlen(argStart + 1) >= ARG_MAX)
            {
                return ERR_CMD_OR_ARGS_TOO_BIG;
            }
            strncpy(clist->commands[cmdCount].args, argStart + 1, ARG_MAX - 1);
        }
        
        strncpy(clist->commands[cmdCount].exe, input, EXE_MAX - 1);
        clist->num++;
        cmdCount++;
        
        input = strtok(NULL, PIPE_STRING);
    }
    
    return OK;
}
