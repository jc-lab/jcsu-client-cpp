/**
 * @file	latest-version.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/11
 * @copyright Copyright (C) 2019 jc-lab. All rights reserved.
 */

#ifndef __JCSU_LATEST_VERSION_H__
#define __JCSU_LATEST_VERSION_H__

#include <stdint.h>

#include <jcu-http/response.h>
#include <jcu-http/request.h>

#include "jcsu-intl/request.h"

namespace jcsu {

    class LatestVersionResponse {
    private:
        bool error_;
        int status_code_;
        uint64_t vid_;
        uint64_t version_number_;
        std::string version_display_;

        std::string response_;

    public:
        class Future {
        private:
            Client *client_;
            std::shared_ptr<jcu::http::ResponseFuture> req_future_;

        public:
            Future(std::shared_ptr<jcu::http::ResponseFuture> req_future);

            void wait() {
                req_future_->wait();
            }

            template<typename _Rep, typename _Period>
            std::future_status wait_for(const std::chrono::duration<_Rep, _Period>& __rel) {
                return req_future_->wait_for(__rel);
            }

            std::unique_ptr<LatestVersionResponse> get();
        };

        LatestVersionResponse(bool err, int status_code, const std::string& response);
        LatestVersionResponse(uint64_t vid, uint64_t version_number, const std::string &version_display);

        bool hasError() const;
        uint64_t getVid() const;
        uint64_t getVersionNumber() const;
        const std::string &getVersionDisplay() const;
        const std::string &getResponse() const;
    };

    class LatestVersionRequest : public Request {
    public:
        std::unique_ptr<LatestVersionResponse::Future> execute(std::shared_ptr<Client> client);
    };

}

#endif //__JCSU_LATEST_VERSION_H__
