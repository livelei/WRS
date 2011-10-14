/*
 * White Rabbit Switch CLI (Command Line Interface)
 * Copyright (C) 2011, CERN.
 *
 * Authors:     Miguel Baizan   (miguel.baizan@integrasys.es)
 *
 * Description: Basic CLI implementation.
 *              It has been built taking into account the CISCO's CLI command
 *              syntax (given that it has become an industry
 *              standard for the management of network devices). However, in WR
 *              we don't envisage to use the CLI as the front-end for managing
 *              the operating system, so some simplifications have been
 *              introduced.
 *
 *              Part of the code written here is based on ideas taken from the
 *              LGPL libcli package (https://github.com/dparrish/libcli.git)
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef __WHITERABBIT_CLI_H
#define __WHITERABBIT_CLI_H

#define VERSION "1.0"

/* Define return codes. Codes minor than 0 means a fatal error so the CLI must
   shutdown */
#define CLI_ERROR           -1  /* Generic error */
#define CLI_ALLOC_ERROR     -2  /* Memory allocation failed */
#define CLI_SNMP_INIT_ERROR -3  /* SNMP initialization error */
#define CLI_OK              0   /* Success */

/* Prompt */
#define EXEC_PROMPT  "# "

/* Default hostname */
#define DEFAULT_HOSTNAME    "WR-Switch"

/* Define a maximum number of commands allowed per line */
#define MAX_CMDS_IN_LINE    5

/* Define a maximum length for the commands inserted by the user (meassured in
   characters) */
#define MAX_CMD_LENGTH      256


/* Main data structures */
struct cli_cmd;
struct cli_shell;

/* Command information */
struct cli_cmd {
    char    *name;          /* Absolute name of the command */
    char    *desc;          /* Description of the command */
    int     opt;            /* Flag to know if this command accepts arguments */
    char    *opt_desc;      /* Description for the arguments */

    int (*handler) (struct cli_shell *, int, char **); /* Handler for the
                                                          command */

    struct cli_cmd *child;      /* Next possible commands in the line */
    struct cli_cmd *parent;     /* Previous command in the line */
    struct cli_cmd *next;       /* Commands with the same parent */
};


/* CLI interpreter information */
struct cli_shell {
    char    *hostname;          /* Name of the host */
    char    *prompt;            /* Prompt for the cli shell */
    int     error;              /* Code for errors */

    struct cli_cmd *root_cmd;    /* Reference to the root of the command tree */
};


/* Functions */
struct cli_shell *cli_init(void);
void cli_build_prompt(struct cli_shell *cli);
struct cli_cmd *cli_find_command(struct cli_shell *cli,
                                 struct cli_cmd *top_cmd,
                                 char *cmd);
void cli_run_command(struct cli_shell *cli, char *string);
void cli_print_error(struct cli_shell *cli);
void cli_main_loop(struct cli_shell *cli);


#endif /*__WHITERABBIT_CLI_H*/
