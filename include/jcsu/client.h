/**
 * @file	client.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/11
 * @copyright Copyright (C) 2019 jc-lab. All rights reserved.
 */

#ifndef __JCSU_CLIENT_H__
#define __JCSU_CLIENT_H__

#include <string>
#include <memory>

#include <jcu-http/client.h>
#include <jcu-http/custom-handler.h>

#include "./jcsu-intl/identity.h"
#include "./jcsu-intl/uniobj.h"

#include "client-handler.h"

#include "latest-version.h"
#include "latest-version.h"

namespace jcsu {

    class SignHeader;

	class Client {
	public:
		class Builder {
		private:
		    std::unique_ptr<Builder> self_;

			std::shared_ptr<Client>* target_;

            std::shared_ptr<jcu::http::Client> http_client_;

			std::string api_endpoint_;
            UniObj<ClientHandler> version_resolver_;

            Builder();
			Builder(std::shared_ptr<Client> *target);

			Client* _build();

            void build(identity<void>)
            {
                if (target_) {
                    *target_ = std::unique_ptr<Client>(_build());
                }
            }

            std::unique_ptr<Client> build(identity<std::unique_ptr<Client>>)
            {
                return std::unique_ptr<Client>(_build());
            }

            std::shared_ptr<Client> build(identity<std::shared_ptr<Client>>)
            {
                return std::shared_ptr<Client>(_build());
            }

		public:
            static Builder &createStaticBuilder(std::shared_ptr<Client> *target);
            static std::unique_ptr<Builder> createBuilder();

            Builder& withHttpClient(std::shared_ptr<jcu::http::Client> client);
			Builder& withApiEndpoint(const char *url);
            Builder& withVersionResolver(ClientHandler *version_resolver);
            Builder& withVersionResolver(std::unique_ptr<ClientHandler> version_resolver);
            Builder& withVersionResolver(std::shared_ptr<ClientHandler> version_resolver);

			template<class T>
			T build() {
                return build(identity<T>());
			}
		};

		static Builder& singletoneBuilder();

		static std::shared_ptr<Client> getSingletone();

		void setDeployId(const std::string& deploy_id);
		const std::string& getDeployId() const;

		// Internal methods
		std::shared_ptr<jcu::http::ResponseFuture> executeHttp(std::unique_ptr<jcu::http::Request> req, jcu::http::CustomHandler *handler = NULL);
        bool verifySignHeader(SignHeader *sign_header);

	private:
	    static std::shared_ptr<Client> singletone_instance_;

	    std::shared_ptr<jcu::http::Client> http_client_;
		UniObj<ClientHandler> client_handler_;

        std::string deploy_id_;

        Client(std::shared_ptr<jcu::http::Client> http_client, std::string api_endpoint, UniObj<ClientHandler> &&client_handler);
	};

}

#endif //__JCSU_CLIENT_H__
