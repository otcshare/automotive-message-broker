/*****************************************************************
 * INTEL CONFIDENTIAL
 * Copyright 2011 - 2013 Intel Corporation All Rights Reserved.
 * 
 * The source code contained or described herein and all documents related to the
 * source code("Material") are owned by Intel Corporation or its suppliers or
 * licensors.Title to the Material remains with Intel Corporation or its
 * suppliers and licensors.The Material may contain trade secrets and proprietary
 * and confidential information of Intel Corporation and its suppliers and
 * licensors, and is protected by worldwide copyright and trade secret laws and
 * treaty provisions.No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or disclosed
 * in any way without Intels prior express written permission.
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise.Any license under such intellectual property rights must be
 * express and approved by Intel in writing. 
 * 
 * Unless otherwise agreed by Intel in writing, you may not remove or alter this
 * notice or any other notice embedded in Materials by Intel or Intels suppliers
 * or licensors in any way.
 *****************************************************************/

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

    DEBUG_CONF("ut_cangenplugin",
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
