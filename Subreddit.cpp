#include <nlohmann/json.hpp>

#include "Subreddit.h"
#include "Post.h"

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
    nlohmann::json response = _redditinstance->_sendrequest("GET",
                                                            "/r/" + name + "/" + sort
                                                            )["data"]["children"];

    std::vector<Post> postvector = {};

    for (short i = 0; i < limit; i++) {
        if (response[i].is_null()) {
            // API returned fewer posts than we asked for
            break;
        }
        Post post = Post(response[i]["data"], _redditinstance);
        postvector.push_back(post);
    }
    return postvector;


}