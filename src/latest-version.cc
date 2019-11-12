/**
 * @file	latest-version.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/11
 * @copyright Copyright (C) 2019 jc-lab. All rights reserved.
 */

#include "jcsu/latest-version.h"
#include "jcsu/client.h"

#include <jcu-http/http-get.h>

#include "rapidjson/document.h"

namespace jcsu {

    std::unique_ptr<jcsu::LatestVersionResponse::Future> LatestVersionRequest::execute(std::shared_ptr<Client> client) {
        std::unique_ptr<jcu::http::Request> req = jcu::http::HttpGet::create("/api/v1/client/d/" + client->getDeployId() + "/latest-version");
        std::shared_ptr<jcu::http::ResponseFuture> res_future = client->executeHttp(std::move(req));
        return std::unique_ptr<LatestVersionResponse::Future>(new LatestVersionResponse::Future(res_future));
    }

    LatestVersionResponse::LatestVersionResponse(bool err, int status_code, const std::string& response)
    : error_(err), status_code_(status_code), response_(response) {}

    LatestVersionResponse::LatestVersionResponse(uint64_t vid,
                                                 uint64_t version_number,
                                                 const std::string &version_display)
        : error_(false), vid_(vid), version_number_(version_number), version_display_(version_display) {}

    bool LatestVersionResponse::hasError() const {
        return error_;
    }
    uint64_t LatestVersionResponse::getVid() const {
        return vid_;
    }
    uint64_t LatestVersionResponse::getVersionNumber() const {
        return version_number_;
    }
    const std::string &LatestVersionResponse::getVersionDisplay() const {
        return version_display_;
    }
    const std::string &LatestVersionResponse::getResponse() const {
        return response_;
    }

    LatestVersionResponse::Future::Future(std::shared_ptr<jcu::http::ResponseFuture> req_future)
    : req_future_(req_future)
    {
    }

    std::unique_ptr<LatestVersionResponse> LatestVersionResponse::Future::get() {
        std::unique_ptr<jcu::http::Response> response(req_future_->get());
        const std::vector<unsigned char> &raw_buf = response->getRawBody();

        if(response->getStatusCode() == 200) {
            rapidjson::Document doc;
            doc.Parse((const char *) raw_buf.data(), raw_buf.size());

            uint64_t vid = doc["vid"].GetUint64();
            uint64_t version_number = doc["version_number"].GetUint64();
            std::string version_display =
                std::string(doc["version_display"].GetString(), doc["version_display"].GetStringLength());

            return std::unique_ptr<LatestVersionResponse>(new LatestVersionResponse(
                vid, version_number, version_display
            ));
        }else{
            return std::unique_ptr<LatestVersionResponse>(new LatestVersionResponse(
                true, response->getStatusCode(), std::string((const char *) raw_buf.data(), raw_buf.size())
            ));
        }
    }

}
