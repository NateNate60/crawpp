#include <restclient-cpp/restclient.h>
#include <restclient-cpp/connection.h>
#include <nlohmann/json.hpp>

#include "Redditor.h"
#include "crawexceptions.hpp"

using namespace CRAW;

Redditor::Redditor (const std::string & name, Reddit * redditinstance) {
    
    nlohmann::json responsejson;
    std::string url = "https://api.reddit.com/user/";
    url += name;
    url += "/about.api";

    RestClient::Response response = RestClient::get(url);
    switch (response.code){
        case 404:
            throw NotFoundError("Could not find any user with that username.");
        case 403:
            throw UnauthorisedError("You aren't allowed to do that. Did you block this user?");
        case 200:
            responsejson = nlohmann::json::parse(response.body)["data"];
            break;
        default:
            throw CommunicationError("The server responded with error " + std::to_string(response.code) + " while fetching Redditor information.");
    }
    
    if (responsejson.is_null()) {
        throw CommunicationError("The server gave a malformed response when fetching Redditor information.");
    }

    _redditinstance = redditinstance;
    username = responsejson["name"];
    created = responsejson["created"];
    totalkarma = responsejson["total_karma"];
    awardeekarma = responsejson["awardee_karma"];
    awarderkarma = responsejson["awarder_karma"];
    commentkarma = responsejson["comment_karma"];
    postkarma = responsejson["link_karma"];
    information = responsejson;
}

std::string Redditor::operator[] (const std::string & attribute) {
    nlohmann::json value = information[attribute];
    if (value.is_null()) {
        throw std::invalid_argument("Attribute " + attribute + " doesn't exist.");
    }
    if (value.is_boolean()) {
        return value ? "true" : "false";
    }
    if (value.is_number()) {
        return std::to_string((double) value);
    }
    if (value.is_string()) {
        return value;
    }
    // This means the value is an object or an array (which is technically also an object)
    // return it as a JSON-string
    return value.dump();
}

void Redditor::follow () {
    if (_redditinstance == nullptr) {
        throw NotLoggedInError("You must be logged in to follow someone.");
    }
    nlohmann::json body;
    body["action"] = "follow";
    body["action_source"] = "g"; // This is arbitrary; IDK why the API requires it
    body["skip_initial_defaults"] = true;
    body["sr_name"] = "u_" + username;

    _redditinstance->_sendrequest("POST", "/api/subscribe", body.dump());
}

void Redditor::unfollow () {
    if (_redditinstance == nullptr) {
        throw NotLoggedInError("You must be logged in to follow someone.");
    }
    nlohmann::json body;
    body["action"] = "unfollow";
    body["action_source"] = "g"; // This is arbitrary; IDK why the API requires it
    body["skip_initial_defaults"] = true;
    body["sr_name"] = "u_" + username;

    _redditinstance->_sendrequest("POST", "/api/subscribe", body.dump());
}