/*
Copyright (C) 2011 Matthew Arsenault

This file is part of Milkway@Home.

Milkyway@Home is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Milkyway@Home is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Milkyway@Home.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "nbody_gl.h"
#include "nbody_graphics.h"
#include "milkyway_util.h"

static const VisArgs defaultVisArgs =
{
    /* .fullscreen     */ FALSE,
    /* .width          */ 0,
    /* .height         */ 0,
    /* .monochrome     */ FALSE,
    /* .notUseGLPoints */ FALSE,
    /* .originCenter   */ FALSE,
    /* .pid            */ 0,
    /* .file           */ NULL,
    /* .key            */ -1
};

static void freeVisArgs(VisArgs* args)
{
    free(args->file);
}


static int handleVisArguments(int argc, const char** argv, VisArgs* visOut)
{
    poptContext context;
    int failed = FALSE;
    static VisArgs visArgs;

    static const struct poptOption options[] =
    {
        {
            "fullscreen", 'f',
            POPT_ARG_NONE, &visArgs.fullscreen,
            0, "Start in fullscreen mode", NULL
        },

        {
            "width", 'w',
            POPT_ARG_INT, &visArgs.width,
            0, "Starting width of window", NULL
        },

        {
            "height", 'h',
            POPT_ARG_INT, &visArgs.height,
            0, "Starting height of window", NULL
        },

        {
            "monochromatic", 'm',
            POPT_ARG_NONE, &visArgs.monochrome,
            0, "All particles have same color", NULL
        },

        {
            "not-use-gl-points", 'n',
            POPT_ARG_NONE, &visArgs.notUseGLPoints,
            0, "Use faster but possibly uglier drawing", NULL
        },

        {
            "origin-center", 'o',
            POPT_ARG_NONE, &visArgs.originCenter,
            0, "Focus on the galactic center instead of system's center of mass", NULL
        },

        {
            "pid", 'p',
            POPT_ARG_INT, &visArgs.pid,
            0, "PID of graphics process to attach to", NULL
        },

        {
            "input-file", 'i',
            POPT_ARG_STRING, &visArgs.file,
            0, "Input file to accompany pid (input file of main process)", NULL
        },


        {
            "key", 'k',
            POPT_ARG_INT, &visArgs.key,
            0, "shm key to attach to", NULL
        },

        POPT_AUTOHELP
        POPT_TABLEEND
    };

    /* TODO: Check project prefs */

    visArgs = defaultVisArgs;
    context = poptGetContext(argv[0], argc, argv, options, 0);

    if (mwReadArguments(context) < 0)
    {
        poptPrintHelp(context, stderr, 0);
        failed = TRUE;
    }
    poptFreeContext(context);

    if ((visArgs.pid != 0) && (visArgs.key > 0))
    {
        /* Both pid and key specified, only want one */
        warn("Can only specify either pid or shm key\n");
        failed = TRUE;
    }

    if (visArgs.pid != 0 && !visArgs.file)
    {
        failed = TRUE;
    }

    *visOut = visArgs;
    return failed;
}

static int nbodyGraphicsInit()
{
#if BOINC_APPLICATION
    if (boinc_parse_init_data_file())
    {
        warn("Error parsing init data file\n");
    }

    if (mwBoincInit(MW_GRAPHICS))
        return 1;
#endif /* BOINC_APPLICATION */

    return 0;
}

int main(int argc, char* argv[])
{
    int rc;
    VisArgs flags;

    if (nbodyGraphicsInit())
        return 1;

    glutInit(&argc, argv);

    if (handleVisArguments(argc, (const char**) argv, &flags))
    {
        freeVisArgs(&flags);
        return 1;
    }

    if (setShmemKey(&flags))
        return 1;

    if (connectSharedScene())
        return 1;

    if (checkConnectedVersion())
        return 1;

    rc = nbodyGLSetup(&flags);
    if (rc)
    {
        warn("Failed to setup nbody graphics\n");
        freeVisArgs(&flags);
        mw_finish(rc);
    }

    glutMainLoop();
    freeVisArgs(&flags);

    return 0;
}

