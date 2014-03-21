/*
Copyright (C) 2012 Intel Corporation

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <glib-object.h>
#include <CppUTest/CommandLineTestRunner.h>
#include "logger.h"

struct ThreadData{
    int argc;
    char** argv;
    int testResult;
    GMainLoop* mainLoop;
};

static void* tests_thread(void *data)
{
    ThreadData* td = static_cast<ThreadData*>(data);
    if(td){
        td->testResult = CommandLineTestRunner::RunAllTests(td->argc, td->argv);
        g_main_loop_quit(td->mainLoop);
    }
    return td;
}

int main(int ac, char **av)
{
    ThreadData td;
    td.argc = ac;
    td.argv = av;
    td.testResult = 0;

    DEBUG_CONF("ut_cansimplugin",
#ifdef _DEBUG
          CUtil::Logger::file_on|CUtil::Logger::screen_on,
#else
          CUtil::Logger::file_on|CUtil::Logger::screen_off,
#endif
          CUtil::Logger::EInfo, CUtil::Logger::EWarning);

    td.mainLoop = g_main_loop_new(NULL, FALSE);

    // Run the mainloop and the tests thread
    GThread* testThread = g_thread_new("tests thread", &tests_thread, &td);

    g_main_loop_run(td.mainLoop);
    g_thread_join(testThread);

    g_main_loop_unref(td.mainLoop);


    return td.testResult;
}
