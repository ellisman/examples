/*********************************************************************
Example source code for using the argtable2 library to implement:

   mv [-bfiuv] [--backup=[CONTROL]] [--reply={yes,no,query}]
   [--strip-trailing-slashes] [-S SUFFIX] [--target-directory=DIRECTORY]
   [--help] [--version] SOURCE [SOURCE]... DEST|DIRECTORY

This file is part of the argtable2 library.
Copyright (C) 1998,1999,2000,2001,2003,2004,2005 Stewart Heitmann
sheitmann@users.sourceforge.net

The argtable2 library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

This software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.
**********************************************************************/

#include "argtable2.h"

int mymain(const char *backup_control,
            int backup,
            int force,
            int interactive,
            const char *reply,
            int strip_trailing_slashes,
            const char *suffix,
            const char *targetdir,
            int update,
            int verbose,
            const char **files,
            int nfiles)
    {
    int j;

    /* if verbose option was given then display all option settings */
    if (verbose)
        {
        printf("backup = %s\n", ((backup)?"YES":"NO"));
        printf("backup CONTROL = %s\n", backup_control);
        printf("force = %s\n", ((force)?"YES":"NO"));
        printf("interactive mode = %s\n", ((interactive)?"YES":"NO"));
        printf("reply = %s\n", reply);
        printf("strip-trailing-slashes = %s\n", ((strip_trailing_slashes)?"YES":"NO"));
        printf("suffix = %s\n", suffix);
        printf("target-directory = %s\n", targetdir);
        printf("update = %s\n", ((update)?"YES":"NO"));
        printf("verbose = %s\n", ((verbose)?"YES":"NO"));
        }

    /* print the source filenames */
    for (j=0; j<nfiles; j++)
        printf("file[%d] = \"%s\"\n", j, files[j]);

    return 0;
    }


int main(int argc, char **argv)
    {
    const char *progname = "mv";
    struct arg_str  *backupc  = arg_str0(NULL, "backup", "[CONTROL]",    "make a backup of each existing destination file");
    struct arg_lit  *backup   = arg_lit0("b", NULL,                      "like --backup but does not accept an argument");
    struct arg_lit  *force    = arg_lit0("f", "force",                   "do not prompt before overwriting");
    struct arg_rem  *force1   = arg_rem (NULL,                           "  equivalent to --reply=yes");
    struct arg_lit  *interact = arg_lit0("i", "interactive",             "Prompt before overwriting");
    struct arg_rem  *interact1= arg_rem (NULL,                           "  equivalent to --reply=yes");
    struct arg_str  *reply    = arg_str0(NULL,"reply", "{yes,no,query}", "specify how to handle the prompt about an");
    struct arg_rem  *reply1   = arg_rem (NULL,                           "  existing destination file");
    struct arg_lit  *strpslsh = arg_lit0(NULL,"strip-trailing-slashes",  "remove any trailing slashes from each SOURCE argument");
    struct arg_str  *suffix   = arg_str0("S", "suffix", "SUFFIX",        "override the usual backup suffix");
    struct arg_str  *targetd  = arg_str0(NULL,"target-directory", "DIRECTORY",  "move all SOURCE arguments into DIRECTORY");
    struct arg_lit  *update   = arg_lit0("u", "update",                  "copy only when the SOURCE file is newer");
    struct arg_rem  *update1  = arg_rem (NULL,                           "  than the destination file or when the");
    struct arg_rem  *update2  = arg_rem (NULL,                           "  destination file is missing");
    struct arg_lit  *verbose  = arg_lit0("v", "verbose",                 "explain what is being done");
    struct arg_lit  *help     = arg_lit0(NULL,"help",                    "display this help and exit");
    struct arg_lit  *version  = arg_lit0(NULL,"version",                 "display version information and exit");
    struct arg_file *files    = arg_filen(NULL, NULL, "SOURCE", 1, argc+2, NULL);
    struct arg_rem  *dest     = arg_rem ("DEST|DIRECTORY", NULL);
    struct arg_end  *end      = arg_end(20);
    void* argtable[] = {backupc,backup,force,force1,interact,interact1,reply,reply1,strpslsh,suffix,targetd,update,update1,update2,verbose,help,version,files,dest,end};
    int exitcode=0;
    int nerrors;

    /* verify the argtable[] entries were allocated sucessfully */
    if (arg_nullcheck(argtable) != 0)
        {
        /* NULL entries were detected, some allocations must have failed */
        printf("%s: insufficient memory\n",progname);
        exitcode=1;
        goto exit;
        }

    /* Set default argument values prior to parsing */
    backupc->sval[0] = "existing";  /* --backup={none,off,numbered,t,existing,nil,simple,never} */
    suffix->sval[0]  = "~";         /* --suffix=~ */
    reply->sval[0]   = "query";     /* --reply={yes,no,query} */
    targetd->sval[0] = NULL;

    /* Parse the command line as defined by argtable[] */
    nerrors = arg_parse(argc,argv,argtable);

    /* special case: '--help' takes precedence over error reporting */
    if (help->count > 0)
        {
        printf("Usage: %s", progname);
        arg_print_syntax(stdout,argtable,"\n");
        printf("Rename SOURCE to DEST, or move SOURCE(s) to DIRECTORY.\n\n");
        arg_print_glossary(stdout,argtable,"  %-30s %s\n");
        printf("\nThe backup suffix is \"~\", unless set with --suffix or SIMPLE_BACKUP_SUFFIX.\n"
               "The version control method may be selected via the --backup option or through\n"
               "the VERSION_CONTROL environment variable.  Here are the values:\n\n"
               "  none, off       never make backups (even if --backup is given)\n"
               "  numbered, t     make numbered backups\n"
               "  existing, nil   numbered if numbered backups exist, simple otherwise\n"
               "  simple, never   always make simple backups\n\n"
               "Report bugs to <foo@bar>.\n");
        exitcode=0;
        goto exit;
        }

    /* special case: '--version' takes precedence error reporting */
    if (version->count > 0)
        {
        printf("'%s' example program for the \"argtable\" command line argument parser.\n",progname);
        printf("September 2003, Stewart Heitmann\n");
        exitcode=0;
        goto exit;
        }

    /* If the parser returned any errors then display them and exit */
    if (nerrors > 0)
        {
        /* Display the error details contained in the arg_end struct.*/
        arg_print_errors(stdout,end,progname);
        printf("Try '%s --help' for more information.\n",progname);
        exitcode=1;
        goto exit;
        }

    /* Command line parsing is complete, do the main processing */
    exitcode = mymain(backupc->sval[0],
                      backup->count,
                      force->count,
                      interact->count,
                      reply->sval[0],
                      strpslsh->count,
                      suffix->sval[0],
                      targetd->sval[0],
                      update->count,
                      verbose->count,
                      files->filename,
                      files->count);

exit:
    /* deallocate each non-null entry in argtable[] */
    arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));

    return exitcode;
    }
