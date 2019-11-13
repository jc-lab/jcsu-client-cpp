/**
 * @file	latest-version.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/11
 * @copyright Copyright (C) 2019 jc-lab. All rights reserved.
 */

#include "jcsu/latest-version.h"
#include "jcsu/client.h"

#include <jcu-http/http-get.h>

#include <json/json.h>

namespace jcsu {

    std::future<std::unique_ptr<LatestVersionResponse>> LatestVersionRequest::execute(std::shared_ptr<Client> client) {
        std::shared_ptr<jcu::http::Request> req = jcu::http::HttpGet::create("/api/v1/client/d/" + client->getDeployId() + "/latest-version");
        std::shared_ptr<LatestVersionResponse::WorkingContext> inst(new LatestVersionResponse::WorkingContext());
        client->prepareHttp(req)
            .withUserContext(inst)
            .onDone([](jcu::http::Response *response) -> void {
              std::shared_ptr<void> user_ctx = response->getUserContext();
              LatestVersionResponse::WorkingContext *work_ctx = (LatestVersionResponse::WorkingContext*)user_ctx.get();
              work_ctx->onDone(response);
            })
            .execute();
        return std::move(inst->promise_.get_future());
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

    LatestVersionResponse::WorkingContext::WorkingContext()
    {
    }

    void LatestVersionResponse::WorkingContext::onDone(jcu::http::Response *response) {
        const std::vector<unsigned char> &raw_buf = response->getRawBody();

        if(response->getStatusCode() == 200) {
            Json::Value doc;
            std::string res_str(raw_buf.begin(), raw_buf.end());
            std::stringstream ss(res_str);

            ss >> doc;
            
            uint64_t vid = doc["vid"].asUInt64();
            uint64_t version_number = doc["version_number"].asUInt64();
            std::string version_display = doc["version_display"].asString();

            promise_.set_value(std::unique_ptr<LatestVersionResponse>(new LatestVersionResponse(
                vid, version_number, version_display
            )));
        }else{
            promise_.set_value(std::unique_ptr<LatestVersionResponse>(new LatestVersionResponse(
                true, response->getStatusCode(), std::string((const char *) raw_buf.data(), raw_buf.size())
            )));
        }
    }

}
