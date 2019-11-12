/**
 * @file	sign-header.h
 * @author	Joseph Lee <development@jc-lab.net>
 * @date	2019/11/12
 * @copyright Copyright (C) 2019 jc-lab. All rights reserved.
 */

#ifndef __JCSU_SIGN_HEADER_H__
#define __JCSU_SIGN_HEADER_H__

#include <string>
#include <vector>

namespace jcsu {

    class SignHeader {
    protected:
        std::string alg_;
        std::vector<std::string> x5c_;
        std::string x5t_;

    public:
        const std::string &getAlg() const {
            return alg_;
        }
        const std::string &getX5T() const {
            return x5t_;
        }
        int x5cCount() const {
            return x5c_.size();
        }
        const std::string& x5cItem(int index) const {
            return x5c_[index];
        }

        virtual bool verifyWithCertificate(const std::string& pem) = 0;
        virtual bool verifyWithCertificate(const std::vector<char>& der) = 0;
    };

}

#endif //__JCSU_CLIENT_HANDLER_H__
