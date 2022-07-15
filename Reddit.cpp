#include <restclient-cpp/connection.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

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
    
    if (this->_instancecount == 0) {
        RestClient::init();
    }
    this->_instancecount++;


    this->username = user_name;
    this->useragent = user_agent;
    this->clientid = client_id;
    this->_apisecret = api_secret;
    this->_password = password;
    this->authenticated = true;
    this->_modhash = "";

    _gettoken();
    _getmodhash();

}


Reddit::Reddit () {
    this->username = "";
    this->useragent = "";
    this->clientid = "";
    this->_apisecret = "";
    this->_password = "";
    this->_token = "";
}

Reddit::~Reddit () {
    this->_instancecount--;
    if (this->_instancecount == 0) {
        RestClient::disable();
    }
}

void Reddit::_gettoken () {

    if (_token != "" && _expiration > time(nullptr) + 5) {
        // we already have a valid token that isn't expiring
        return;
    }

    RestClient::Connection connection = RestClient::Connection ("https://www.reddit.com/api/v1/access_token");
    connection.SetBasicAuth(clientid, _apisecret);
    connection.SetUserAgent(useragent);
    RestClient::Response response = connection.post("/", 
                                                    "{\"grant_type\": \"password\", \"username\": \"" +
                                                    username +
                                                    "\", \"password\": \"" +
                                                    _password
                                                    + "\"}");
    nlohmann::json responsejson = nlohmann::json::parse(response.body);
    if (response.code != 200 || !responsejson["error"].is_null()) {
        _instancecount--;
        if (_instancecount == 0) {
            RestClient::disable();
        }
        throw LoginError("An error occurred when attempting to retrieve a token: " + (std::string)responsejson["message"]);
    }

    _token = responsejson["access_token"];
    _expiration = (time_t)responsejson["expires_in"] + time(nullptr);
}

void Reddit::_getmodhash () {
    nlohmann::json response = _sendrequest("GET", "/api/me.json");
    _modhash = response["data"]["modhash"];
}

nlohmann::json Reddit::_sendrequest (const std::string & method, 
                                     const std::string & targeturl, 
                                     const std::string & body) {

    if (time(nullptr) >= _expiration - 5) {
        // the token is expiring soon, get another one
        _gettoken();
    }

    RestClient::Connection connection = RestClient::Connection ("https://api.reddit.com");
    connection.FollowRedirects(true);
    connection.AppendHeader("Authorization", "bearer " + _token);
    if (_modhash != "") {
        connection.AppendHeader("X-Modhash", _modhash);
    }
    RestClient::Response response;
    if (method == "GET") {
        response = connection.get(targeturl);
    } else if (method == "POST") {
        response = connection.post(targeturl, body);
    } else if (method == "PUT") {
        response = connection.put(targeturl, body);
    } else if (method == "DELETE") {
        response = connection.del(targeturl);
    } else {
        throw std::invalid_argument(method + " is not a recognised HTTP method.");
    }
    return nlohmann::json::parse(response.body);
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

