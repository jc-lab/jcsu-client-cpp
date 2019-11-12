/**
 * @file	file-list.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/11
 * @copyright Copyright (C) 2019 jc-lab. All rights reserved.
 */

#include "jcsu/file-list.h"
#include "jcsu/client.h"

#include <sstream>
#include <map>

#include <jcu-http/http-get.h>

#include <josepp/jws.hpp>

#include <json/json.h>

#include "sign-header-impl.h"

namespace jcsu {

    FileListRequest &FileListRequest::withVid(uint64_t vid) {
        vid_ = vid;
        return *this;
    }

    std::unique_ptr<FileListResponse::Future> FileListRequest::execute(std::shared_ptr<Client> client) {
        std::stringstream ss;
        ss << "/api/v1/client/d/" << client->getDeployId() << "/file-list/" << vid_;
        std::unique_ptr<jcu::http::Request> req = jcu::http::HttpGet::create(ss.str());
        std::shared_ptr<jcu::http::ResponseFuture> res_future = client->executeHttp(std::move(req));
        return std::unique_ptr<FileListResponse::Future>(new FileListResponse::Future(res_future, client));
    }

    FileListResponse::FileListResponse(bool err, bool verify_failed, int status_code, const std::string& response)
    : error_(err), verify_failed_(verify_failed), status_code_(status_code), response_(response) {}

    FileListResponse::FileListResponse(uint64_t vid,
    uint64_t version_number,
    const std::string &version_display)
    : error_(false), vid_(vid), version_number_(version_number), version_display_(version_display) {}

    bool FileListResponse::hasError() const {
        return error_;
    }
    bool FileListResponse::isVerifyFailed() const {
        return verify_failed_;
    }
    uint64_t FileListResponse::getVid() const {
        return vid_;
    }
    uint64_t FileListResponse::getVersionNumber() const {
        return version_number_;
    }
    const std::string &FileListResponse::getVersionDisplay() const {
        return version_display_;
    }
    const std::string &FileListResponse::getSignedMetadata() const {
        return signed_metadata_;
    }
    const std::string &FileListResponse::getResponse() const {
        return response_;
    }
    int FileListResponse::getFileCount() const {
        return file_list_.size();
    }
    const FileItem &FileListResponse::getFileItem(int index) const {
        return file_list_[index];
    }

    FileListResponse::Future::Future(std::shared_ptr<jcu::http::ResponseFuture> req_future, std::shared_ptr<Client> client)
    : req_future_(req_future), client_(client)
    {
    }

    std::unique_ptr<FileListResponse> FileListResponse::Future::get() {
        std::unique_ptr<jcu::http::Response> response(req_future_->get());
        const std::vector<unsigned char> &raw_buf = response->getRawBody();

        if(response->getStatusCode() == 200) {
            Json::Value doc;
            std::string res_str(raw_buf.begin(), raw_buf.end());
            std::stringstream ss(res_str);

            std::map<std::string, std::string> url_map;

            ss >> doc;

            uint64_t vid = doc["vid"].asUInt64();
            uint64_t version_number = doc["version_number"].asUInt64();
            std::string version_display = doc["version_display"].asString();
            std::string signed_metadata = doc["signed_metadata"].asString();

            jose::sp_jws jws = jose::jws::parse(signed_metadata, false);
            std::unique_ptr<SignHeaderImpl> sign_header = SignHeaderImpl::fromJws(jws);

            if(!client_->verifySignHeader(sign_header.get())) {
                return std::unique_ptr<FileListResponse>(new FileListResponse(
                    true, true, response->getStatusCode(), "Verify failed"
                ));
            }

            const Json::Value &json_file_list = doc["file_list"];
            for(auto it = json_file_list.begin(); it != json_file_list.end(); it++) {
                url_map[(*it)["file_hash_md5"].asString()] = (*it)["url"].asString();
            }

            const Json::Value *json_distribute_files = jws->claims().get().json("distribute_files");

            std::unique_ptr<FileListResponse> inst(new FileListResponse(
                vid, version_number, version_display
            ));

            inst->file_list_.reserve(json_distribute_files->size());
            for(auto it = json_distribute_files->begin(); it != json_distribute_files->end(); it++) {
                std::string file_hash_md5 = (*it)["file_hash_md5"].asString();
                std::string file_hash_sha256 = (*it)["file_hash_sha256"].asString();
                uint64_t file_size = (*it)["file_size"].asUInt64();
                std::string dist_path = (*it)["dist_path"].asString();
                inst->file_list_.emplace_back(file_hash_md5, file_hash_sha256, file_size, dist_path, url_map[file_hash_md5]);
            }

            return std::move(inst);
        }else{
            return std::unique_ptr<FileListResponse>(new FileListResponse(
                true, false, response->getStatusCode(), std::string((const char *) raw_buf.data(), raw_buf.size())
            ));
        }
    }

}
