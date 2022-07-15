#include <nlohmann/json.hpp>
#include <restclient-cpp/restclient.h>

#include "Subreddit.h"
#include "Post.h"
#include "crawexceptions.hpp"

using namespace CRAW;

Subreddit::Subreddit (const std::string & subredditname, Reddit * redditinstance) {
    information = redditinstance->_sendrequest("GET",
                                        "/r/" + subredditname + "/about"
                                        )["data"];
    name = information["display_name"];
    fullname = information["name"];
    banned = information["banned"];
    postingrestricted = information["restrict_posting"];
    quarantined = information["quarantine"];
    language = information["lang"];
    created = (time_t)information["created"];
    subscribers = (unsigned int)information["subscribers"];
    activeusers = (unsigned int)information["active_user_count"];
}

std::string Subreddit::operator[] (const std::string & attribute) {
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
    RestClient::Response response = RestClient::get("https:/api.reddit.com/r/" + name + "/" + sort);                                                  
    nlohmann::json responsejson = nlohmann::json::parse(response.body)["data"]["children"];
    switch (response.code) {
        case 403:
            throw UnauthorisedError("You don't have permission to look at r/" + name + " posts.");
        case 200:
            break;
        default:
            throw RetrievalError("The server responded to fetching r/" + name + " posts with error code " + std::to_string(response.code));
    }
    if (responsejson.is_null()) {
        throw RetrievalError("Malformed response from server when fetching r/" + name + " posts.");
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