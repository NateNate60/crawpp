#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include <stdexcept>
#include <iostream>

#include "Reddit.h"
#include "Subreddit.h"
#include "Post.h"
#include "Redditor.h"
#include "Comment.h"
#include "crawexceptions.hpp"

using namespace CRAW;

Reddit::Reddit (const std::string & user_name, 
                const std::string & password, 
                const std::string & client_id, 
                const std::string & api_secret, 
                const std::string & user_agent) {
    if (user_agent == "") {
        throw std::invalid_argument("User agent string must not be empty");
    }

    this->username = user_name;
    this->useragent = user_agent;
    this->clientid = client_id;
    this->_apisecret = api_secret;
    this->_password = password;
    this->authenticated = true;

    _gettoken();

}


Reddit::Reddit () {
    this->username = "";
    this->useragent = "";
    this->clientid = "";
    this->_apisecret = "";
    this->_password = "";
    this->_token = "";
}

void Reddit::_gettoken () {

    if (_token != "" && _expiration > time(nullptr) + 5) {
        // we already have a valid token that isn't expiring
        return;
    }

    cpr::Response response = cpr::Post(cpr::Url{"https://www.reddit.com/api/v1/access_token"},
                                       cpr::Authentication{clientid, _apisecret, cpr::AuthMode::BASIC},
                                       cpr::Payload{{"grant_type", "password"}, {"username", username}, {"password", _password}},
                                       cpr::Header{{"User-Agent", useragent}},
                                       cpr::Ssl(cpr::ssl::TLSv1_2())); // this might otherwise default to TLS 1.0
    nlohmann::json responsejson = nlohmann::json::parse(response.text);
    if (response.status_code != 200 || !responsejson["error"].is_null()) {
        std::string errormessage = responsejson["error"];
        throw LoginError("An error occurred when attempting to retrieve a token: " + errormessage);
    }

    _token = responsejson["access_token"];
    _expiration = (time_t)responsejson["expires_in"] + time(nullptr);
}

nlohmann::json Reddit::_sendrequest (const std::string & method, 
                                     const std::string & targeturl, 
                                     const std::string & body) {

    if (time(nullptr) >= _expiration - 5) {
        // the token is expiring soon, get another one
        _gettoken();
    }

    // this might otherwise default to TLS 1.0. TLS 1.2+ is more secure
    cpr::SslOptions tls = cpr::Ssl(cpr::ssl::TLSv1_2());

    cpr::Header header;
    header = {{"User-Agent", useragent}, {"Authorization", "bearer " + _token}};
    
    cpr::Url url = "https://oauth.reddit.com" + targeturl;
    cpr::Response response;
    if (method == "GET") {
        response = cpr::Get(url, header, tls);
    } else if (method == "POST") {
        response = cpr::Post(url, header, cpr::Body{body}, tls);
    } else if (method == "PUT") {
        response = cpr::Put(url, header, cpr::Body{body}, tls);
    } else if (method == "DELETE") {
        response = cpr::Delete(url, header, tls);
    } else {
        throw std::invalid_argument(method + " is not a recognised HTTP method.");
    }
    return nlohmann::json::parse(response.text);
}


Redditor Reddit::me () {
    if (!authenticated) {
        // Throw error, we can't provide info on who "me" is if the user doesn't tell us!
        throw NotLoggedInError("No authentication info provided; cannot determine who \"me\" is.");
    }
    return Redditor(username, this);
}

Redditor Reddit::redditor (const std::string & name) {
    // Associate with the current session only if authenticated
    if (authenticated) {
        return Redditor(name, this);
    }
    return Redditor(name);
}

Subreddit Reddit::subreddit (const std::string & name) {
    // Associate with the current session only if authenticated
    if (authenticated) {
        return Subreddit(name, this);
    } else {
        return Subreddit(name);
    }
}

Post Reddit::post (const std::string & id) {
    // Associate with the current session only if authenticated
    if (authenticated) {
        return Post(id, this);
    } else {
        return Post(id);
    }
}

