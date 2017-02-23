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

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <inter_msg.h>
#include <ptracer_pub.h>
#include <ptracer.h>

int main(int argc, char *argv[])
{
    int result;
    opterr = 0;
    optopt = 0;
    while((result = getopt(argc, argv, "i:p:l:h")) != -1) {
    	switch(result)
    	{
    	case 'i':
    	{
    		//printf("optopt=%c, optarg=%s\n", optopt, optarg);
    		break;
    	}
    	case 'p':
    	{
    		//printf("optopt=%c, optarg=%s\n", optopt, optarg);
    		break;
    	}
    	case 'l':
    	{
    		//printf("optopt=%c, optarg=%s\n", optopt, optarg);
    		break;
    	}
    	case 'h':
    	case '?':
    	{
    		printf("usage: ptracer_xxx.exe [-i <address>] [-p <port>] [-l <loglevel>]\n");
    		break;
    	}
    	default:
    	{
    		//printf("optopt=%c, optarg=%s\n", optopt, optarg);
    		break;
    	}
    	}
	}

	Ptracer *local_ptracer = new Ptracer();
	result = local_ptracer->init(inet_addr(UTER_LOCAL_ADDR), htons(UTER_LOCAL_PORT), 1);

	while(1) {
		sleep(1);
	}
	return 0;
}
