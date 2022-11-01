#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include <stdexcept>

#include "crawpp/Reddit.h"
#include "crawpp/Subreddit.h"
#include "crawpp/Post.h"
#include "crawpp/Redditor.h"
#include "crawpp/Comment.h"
#include "crawpp/Message.h"
#include "crawpp/ListingPage.hpp"
#include "crawpp/crawexceptions.hpp"

namespace CRAW {

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


    Reddit::Reddit (const std::string & user_agent) {
        this->username = "";
        this->useragent = user_agent;
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
            throw errors::LoginError("An error occurred when attempting to retrieve a token: " + errormessage);
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
        cpr::Url url;
        if (authenticated) {
            header = {{"User-Agent", useragent}, {"Authorization", "bearer " + _token}};
            url = "https://oauth.reddit.com" + targeturl;
        } else {
            header = {{"User-Agent", useragent}};
            url = "https://api.reddit.com" + targeturl;
        }

        
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
        switch (response.status_code) {
            case 404:
                throw errors::NotFoundError("Server responded with HTTP 404 (Not Found)");
            case 403:
                throw errors::UnauthorisedError("Server responded with HTTP 403 (Unauthorised)");
            case 200:
                return nlohmann::json::parse(response.text);
            default:
                throw errors::CommunicationError("Server responded with error code " + response.status_code);
        }

        
    }

    nlohmann::json Reddit::_sendrequest (const std::string & method, 
                                         const std::string & targeturl, 
                                         const cpr::Payload & body,
                                         const cpr::Parameters & parameters) {

        if (time(nullptr) >= _expiration - 5) {
            // the token is expiring soon, get another one
            _gettoken();
        }

        // this might otherwise default to TLS 1.0. TLS 1.2+ is more secure
        cpr::SslOptions tls = cpr::Ssl(cpr::ssl::TLSv1_2());

        cpr::Header header;
        cpr::Url url;
        if (authenticated) {
            header = {{"User-Agent", useragent}, {"Authorization", "bearer " + _token}};
            url = "https://oauth.reddit.com" + targeturl;
        } else {
            header = {{"User-Agent", useragent}};
            url = "https://api.reddit.com" + targeturl;
        }

        
        cpr::Response response;
        if (method == "GET") {
            response = cpr::Get(url, header, parameters, tls);
        } else if (method == "POST") {
            response = cpr::Post(url, header, body, parameters, tls);
        } else if (method == "PUT") {
            response = cpr::Put(url, header, body, parameters, tls);
        } else if (method == "DELETE") {
            response = cpr::Delete(url, header, parameters, tls);
        } else {
            throw std::invalid_argument(method + " is not a recognised HTTP method.");
        }
        return nlohmann::json::parse(response.text);
    }



    Redditor Reddit::me () {
        if (!authenticated) {
            // Throw error, we can't provide info on who "me" is if the user doesn't tell us!
            throw errors::NotLoggedInError("No authentication info provided; cannot determine who \"me\" is.");
        }
        return Redditor(username, this);
    }

    Redditor Reddit::redditor (const std::string & name) {
        return Redditor(name, this);
    }

    Subreddit Reddit::subreddit (const std::string & name) {
        return Subreddit(name, this);
    }

    Post Reddit::post (const std::string & id) {
        return Post(id, this);

    }

    std::multiset<std::string> Reddit::search (const std::string & query, bool exact, bool nsfw, bool autocomplete, int limit) {
        if (limit < 0 || limit > 10) {
            throw std::invalid_argument("The limit of results to return must be between 0 and 10.");
        }
        std::string endpoint = autocomplete ? "/api/subreddit_autocomplete_v2" : "/api/search_reddit_names";
        nlohmann::json response = _sendrequest("GET", endpoint, {}, cpr::Parameters {{"exact", exact ? "true": "false"},
                                                                                     {"include_over_18", nsfw ? "true" : "false"},
                                                                                     {"include_unadvertisable", "true"},
                                                                                     {"query", query},
                                                                                     {"limit", std::to_string(limit)}});
        std::multiset<std::string> results;
        if (!autocomplete) {
            for (auto & i : response["names"]) {
                results.insert(i.get<std::string>());
            }
        } else {
            for (auto & i : response["data"]["children"]) {
                results.insert(i["data"]["display_name"].get<std::string>());
            }
        }
        return results;
    }

    std::vector<Message> Reddit::inbox (const std::string & filter, ListingPage listingpage, const std::string & direction) {
        if (filter != "inbox" && filter != "sent" && filter != "unread" && filter != "messages") {
            throw std::invalid_argument("filter must be either \"inbox\", \"unread\", \"sent\", or \"messages\" not " + filter + ".");
        }
        nlohmann::json response = _sendrequest("GET", "/message/" + filter);
        std::vector<Message> inbox;
        for (auto & object : response["data"]["children"]) {
            inbox.emplace_back(Message(object["data"], this));
        }
        return inbox;
    }
}