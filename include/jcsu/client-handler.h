/**
 * @file	client-handler.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/11
 * @copyright Copyright (C) 2019 jc-lab. All rights reserved.
 */

#ifndef __JCSU_CLIENT_HANDLER_H__
#define __JCSU_CLIENT_HANDLER_H__

#include <stdint.h>
#include "sign-header.h"

namespace jcsu {

	class ClientHandler {
	public:
		virtual uint64_t getCurrentVersion() = 0;
		virtual bool verify(SignHeader *sign_header) = 0;
	};

}

#endif //__JCSU_CLIENT_HANDLER_H__
