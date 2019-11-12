/**
 * @file	client.cc
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/11
 * @copyright Copyright (C) 2019 jc-lab. All rights reserved.
 */

#include "jcsu/client.h"

#include <jcu-http/default-cacerts.h>
#include <jcu-http/http-get.h>

namespace jcsu {

    std::shared_ptr<Client> Client::singletone_instance_;

    Client::Client(std::shared_ptr<jcu::http::Client> http_client, std::string api_endpoint, UniObj<ClientHandler> &&client_handler) {
        http_client_ = http_client;
        if(!http_client_) {
            http_client_ = jcu::http::Client::create();
            http_client_->addTrustCACert(jcu::http::defaultCacerts());
        }
        http_client_->setApiEndpoint(api_endpoint);
        client_handler_ = std::move(client_handler);
    }

    Client::Builder& Client::singletoneBuilder() {
        return Builder::createStaticBuilder(&singletone_instance_);
    }

    std::shared_ptr<Client> Client::getSingletone() {
        return singletone_instance_;
    }

    void Client::setDeployId(const std::string& deploy_id) {
        deploy_id_ = deploy_id;
    }

    const std::string& Client::getDeployId() const {
        return deploy_id_;
    }

    std::shared_ptr<jcu::http::ResponseFuture> Client::executeHttp(std::unique_ptr<jcu::http::Request> req, jcu::http::CustomHandler *handler) {
        return http_client_->execute(std::move(req), handler);
    }

    bool Client::verifySignHeader(SignHeader *sign_header) {
        return client_handler_.get()->verify(sign_header);
    }

}
