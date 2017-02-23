/******************************************************************************
 * Copyright(C),2015-2015, Yaozhongkai
 * FileName:
 * Author:Yaozhongkai
 * Version:
 * Date:
 * Description:
 * Others:
 * Function List:
 * History:
 * ****************************************************************************/

#ifndef SRC_UTER_PUBLIC_H_
#define SRC_UTER_PUBLIC_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <uuid/uuid.h>

#include <QtCore/QMap>
#include <QtCore/QList>

#include <string>
using namespace std;

#include <main.hxx>

#define UTER_ARRAY_SIZE(array) (sizeof(array)/sizeof(array[0]))
#define UTER_ARRAY_CHECK(array, len) static char g_array_check_##array[1 - (UTER_ARRAY_SIZE(array) != len) * 2] __attribute__((unused))

extern int uter_log_level;

#define UTER_DUMPING() 	(uter_log_level >= 3)
#define UTER_DEBUGING()	(uter_log_level >= 2)
#define UTER_LOGING() 	(uter_log_level >= 1)

#define UTER_DUMP(fmt, ...)  if(uter_log_level >= 3){ emit g_ui_main.signalUterLog(3, QString::asprintf( "DUMP:"fmt"\n", ##__VA_ARGS__)); }
#define UTER_DEBUG(fmt, ...) if(uter_log_level >= 2){ emit g_ui_main.signalUterLog(2, QString::asprintf("DEBUG:"fmt"\n", ##__VA_ARGS__)); }
#define UTER_LOG(fmt, ...)   if(uter_log_level >= 1){ emit g_ui_main.signalUterLog(1, QString::asprintf(  "LOG:"fmt"\n", ##__VA_ARGS__)); }
#define UTER_PRINT(fmt, ...) if(uter_log_level >= 0){ emit g_ui_main.signalUterLog(0, QString::asprintf(        fmt"\n", ##__VA_ARGS__)); }
#define UTER_STBAR(time,fmt, ...)             emit g_ui_main.signalMsgToStatBar(time, QString::asprintf(        fmt"\n", ##__VA_ARGS__))

#define UTER_ASSERT(x) \
do {\
if (!(x)) {\
	printf("Assert,%s:%d\n", __FILE__, __LINE__);\
	assert(0);\
}\
}while(0)

#define UTER_INVALID_PID (0xffffffff)
#define UTER_INVALID_ADDR (0xffffffffffffffff)

#endif /*SRC_UTER_PUBLIC_H_*/
