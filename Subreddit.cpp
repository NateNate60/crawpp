#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

#include "Subreddit.h"
#include "Post.h"
#include "crawexceptions.hpp"

using namespace CRAW;

Subreddit::Subreddit (const std::string & subredditname, Reddit * redditinstance) {
    if (redditinstance == nullptr) {
        cpr::Response response = cpr::Get(cpr::Url("https://api.reddit.com/r/" + subredditname + "/about"), cpr::Ssl(cpr::ssl::TLSv1_2()));
        switch (response.status_code) {
            case 404:
                throw NotFoundError("Could not find a subreddit with name r/" + subredditname);
            case 403:
                throw UnauthorisedError("You aren't allowed to access r/" + subredditname);
            case 200:
                information = nlohmann::json::parse(response.text)["data"];
                break;
            default:
                throw CommunicationError("The server responded with error " + std::to_string(response.status_code) + " while fetching subreddit information.");
        }
    } else {
        information = redditinstance->_sendrequest("GET", "/r/" + subredditname + "/about");
    }
    _redditinstance = redditinstance;
    name = information["display_name"];
    fullname = information["name"];
    if (information["user_is_banned"].is_null()) {
        banned = false;
    } else {
        banned = information["banned"];
    }
    postingrestricted = information["restrict_posting"];
    quarantined = information["quarantine"];
    language = information["lang"];
    created = static_cast<time_t>(information["created"]);
    subscribers = information["subscribers"];
    activeusers = information["active_user_count"];
}

std::string Subreddit::operator[] (const std::string & attribute) {
    nlohmann::json value = information[attribute];
    if (value.is_null()) {
        throw std::invalid_argument("Attribute " + attribute + " doesn't exist.");
    }
    return value.dump();
}

std::vector<Post> Subreddit::posts (const std::string & sort,
                                    const std::string & period,
                                    const unsigned short limit) {
    if (sort != "hot" &&
        sort != "rising" &&
        sort != "top" &&
        sort != "controversial" &&
        sort != "new") {
            throw std::invalid_argument("Invalid sort type: " + sort);
    } else if ((sort == "top" || sort == "controversial") 
                && ( period != "hour" &&
                period != "day" &&
                period != "week" &&
                period != "month" &&
                period != "year" &&
                period != "all")) {
                    throw std::invalid_argument("Sorting by " + sort + " requires a valid period.");
    }
    cpr::Response response = cpr::Get(cpr::Url("https:/api.reddit.com/r/" + name + "/" + sort), cpr::Ssl(cpr::ssl::TLSv1_2()));                                                  
    switch (response.status_code) {
        case 403:
            throw UnauthorisedError("You don't have permission to look at r/" + name + " posts.");
        case 200:
            break;
        default:
            throw CommunicationError("The server responded to fetching r/" + name + " posts with error code " + std::to_string(response.status_code));
    }
    nlohmann::json responsejson = nlohmann::json::parse(response.text)["data"]["children"];
    if (responsejson.is_null()) {
        throw CommunicationError("Malformed response from server when fetching r/" + name + " posts.");
    }
    std::vector<Post> postvector = {};

    for (short i = 0; i < limit; i++) {
        if (responsejson[i].is_null()) {
            // API returned fewer posts than we asked for
            break;
        }
        Post post = Post(responsejson[i]["data"], _redditinstance);
        postvector.push_back(post);
    }
    return postvector;


}