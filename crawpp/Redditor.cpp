#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "crawpp/Redditor.h"
#include "crawpp/crawexceptions.hpp"

using namespace CRAW;

Redditor::Redditor (const std::string & name, Reddit * redditinstance) {
    
    nlohmann::json responsejson;
    try {
        responsejson = redditinstance->_sendrequest("GET", "/user/" + name + "/about")["data"];
    } catch (const errors::NotFoundError &) {
        throw errors::NotFoundError("Could not find any user with username" + name + ".");
    } catch (const errors::UnauthorisedError &) {
        throw errors::UnauthorisedError("You aren't allowed to access u/" + name + ". Did you block this user?");
    } catch (const errors::CommunicationError & error) {
        throw errors::CommunicationError(error.what() + std::string(" while fetching u/") + name + ".");
    }

    _redditinstance = redditinstance;
    username = responsejson["name"];
    created = responsejson["created"];
    totalkarma = responsejson["total_karma"];
    awardeekarma = responsejson["awardee_karma"];
    awarderkarma = responsejson["awarder_karma"];
    commentkarma = responsejson["comment_karma"];
    postkarma = responsejson["link_karma"];
    fullname = std::string("t2_") + static_cast<std::string>(responsejson["id"]);
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
    if (!_redditinstance->authenticated) {
        throw errors::NotLoggedInError("You must be logged in to follow someone.");
    }
    nlohmann::json body;
    body["action"] = "follow";
    body["action_source"] = "g"; // This is arbitrary; IDK why the API requires it
    body["skip_initial_defaults"] = true;
    body["sr_name"] = "u_" + username;

    _redditinstance->_sendrequest("POST", "/api/subscribe", body.dump());
}

void Redditor::unfollow () {
    if (!_redditinstance->authenticated) {
        throw errors::NotLoggedInError("You must be logged in to follow someone.");
    }
    nlohmann::json body;
    body["action"] = "unfollow";
    body["action_source"] = "g"; // This is arbitrary; IDK why the API requires it
    body["skip_initial_defaults"] = true;
    body["sr_name"] = "u_" + username;

    _redditinstance->_sendrequest("POST", "/api/subscribe", body.dump());
}

void Redditor::block () {
    nlohmann::json body;
    body["name"] = username;
    _redditinstance->_sendrequest("POST", "/api/block_user", cpr::Payload{{"name", username}});
}