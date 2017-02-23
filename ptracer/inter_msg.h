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

#ifndef SRC_PTRACER_INTER_MSG_H_
#define SRC_PTRACER_INTER_MSG_H_

#define UTER_LOCAL_ADDR ("127.0.0.1")
#define UTER_LOCAL_PORT (9090)

enum uter_msg_code {
	PTCR_REQ_DATA,
	PTCR_RESP_DATA,
	PTCR_EVT_DATA,
};

enum uter_msg_child_errno {
	PTCR_CHILD_EXCEPTION,
	PTCR_CHILD_EXIT_TRACEME,
	PTCR_CHILD_EXIT_E2BIG,
	PTCR_CHILD_EXIT_ENAMETOOLONG,
	PTCR_CHILD_EXIT_EACCES,
	PTCR_CHILD_EXIT_ENOENT,
	PTCR_CHILD_EXIT_EISDIR,
	PTCR_CHILD_EXIT_ELOOP,
	PTCR_CHILD_EXIT_ENOEXEC,
	PTCR_CHILD_EXIT_UNKNOWN,
};

inline unsigned int ldata_high(unsigned long long ldata)
{
    return ldata >> 32;
}

inline unsigned int ldata_low(unsigned long long ldata)
{
    return ldata & 0x00000000FFFFFFFF;
}

inline unsigned long long ldata(unsigned int ldata_high, unsigned int ldata_low)
{
    unsigned long long ret = ldata_high;
    return (ret << 32) | ldata_low;
}

struct uter_long_data_msg {
	unsigned int low;
	unsigned int high;
};

#define INTER_MSG_MAX  (9248)
#define PTRACER_ARG_ENV_MAX  (256)
#define PTRACER_REG_NAME_MAX  (16)
#define INTER_MSG_MAGIC  (0xff123456)

//消息头
struct ptr_msg {
	unsigned int magic;					//幻数，请求与应带的幻数是一致的。event 和 operate 的magic等于0。
	unsigned int len;					//len是整个消息的长度，包括消息头和消息体
	unsigned int code;					//消息代码
};

#endif /* SRC_PTRACER_INTER_MSG_H_ */
