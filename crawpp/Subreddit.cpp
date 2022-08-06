#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

#include "crawpp/Subreddit.h"
#include "crawpp/Post.h"
#include "crawpp/Redditor.h"
#include "crawpp/crawexceptions.hpp"

namespace CRAW {

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
            information = redditinstance->_sendrequest("GET", "/r/" + subredditname + "/about")["data"];
            if (!information["children"].is_null()) {
                // the listing having "children" means it's actually a search listing
                // and there isn't a subreddit with that exact name
                std::string similars = "";
                for (auto & i : information["children"]) {
                    similars += " ";
                    similars += i["data"]["display_name"];
                }
                throw NotFoundError("No subreddit named \"" + subredditname + "\" exists. Did you mean any of these?" + similars);
            }
        }
        _redditinstance = redditinstance;
        name = information["display_name"];
        fullname = information["name"];
        if (information["user_is_banned"].is_null()) {
            banned = false;
        } else {
            banned = information["user_is_banned"];
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



    Post Subreddit::post (const std::string & title,
                        const std::string & contents,
                        const PostOptions & options) {
        if (_redditinstance == nullptr) {
            throw NotLoggedInError("You must be logged in to make a post.");
        }
        if (options.event_start == 0 && options.event_end != 0 || options.event_start != 0 && options.event_end == 0) {
            throw PostingError("Either both event_start and event_end must be specified, or neither, but not one.");
        }
        if (options.event_start != 0 && options.event_timezone == "") {
            throw PostingError("A time zone was not specified.");
        }
        cpr::Payload payload = {};
        if (options.event_start != 0) {

            // It's only a maximum of 20 characters long but better safe than sorry
            // buffer overflow begone!!
            char startstring [25];
            char endstring [25];

            // format the string to the desired format, which is YYYY-MM-DDTHH:MM:SS
            strftime(startstring, 24, "%FT%T", localtime(&options.event_start));
            strftime(endstring, 24, "%FT%T", localtime(&options.event_end));

            payload = {{"title", title}, 
                    {"text", contents}, {"url", contents},
                    {"ad", std::to_string(options.ad)},
                    {"collection_id", options.collection_id},
                    {"flair_id", options.flair_id},
                    {"kind", options.type == "text" ? "self" : "link"},
                    {"nsfw", std::to_string(options.nsfw)},
                    {"resubmit", std::to_string(options.resubmit)},
                    {"sendreplies", std::to_string(options.get_inbox_replies)},
                    {"spoiler", std::to_string(options.spoiler)},
                    {"sr", name},
                    {"event_start", startstring},
                    {"event_end", endstring}};
        } else {
            payload = {{"title", title}, 
                    {"text", contents}, {"url", contents},
                    {"ad", std::to_string(options.ad)},
                    {"collection_id", options.collection_id},
                    {"flair_id", options.flair_id},
                    {"kind", options.type == "text" ? "self" : "link"},
                    {"nsfw", std::to_string(options.nsfw)},
                    {"resubmit", std::to_string(options.resubmit)},
                    {"sendreplies", std::to_string(options.get_inbox_replies)},
                    {"spoiler", std::to_string(options.spoiler)},
                    {"sr", name}};
        }

        nlohmann::json response = _redditinstance->_sendrequest("POST", "/api/submit", payload);
        if (!response["status"].is_null()) {
            throw CommunicationError("The server gave a malformed response while attempting to make a post.");
        }
        if (!response["success"]) {

            //server responds with a jquery object
            std::string error = response["jquery"][10][3][0];
            std::string description = response["jquery"][14][3][0];

            throw PostingError("The following error was encountered when attempting to post: \"" +
                            error +
                            "\" - \"" +
                            description +
                            "\"");
        }

        std::string posturl = response["jquery"][10][3][0];
        
        // the goal of this is to isolate the post's ID so we can init a Post object with it
        // posturl is in the form:
        // https://www.reddit.com/r/SUBREDDITNAME/comments/ID/POST_NAME
        //                         |             |        |
        //               position 25             |        |
        //           25 + length of subreddit name        |
        //               25 + length of subreddit name + 10
        //
        // ...and the ID is always 6 characters long
        return Post(posturl.substr(25 + name.length() + 10, 6), _redditinstance);
    }

    Subreddit & Subreddit::subscribe (bool skip_initial_defaults) {
        if (_redditinstance == nullptr) {
            throw NotLoggedInError("You must be logged in to subscribe to r/" + name + ".");
        }

        nlohmann::json body = {};
        body["action"] = "sub";
        body["action_source"] = "a"; //required by API for unknown reason
        body["skip_initial_defaults"] = skip_initial_defaults;
        body["sr"] = name;
        _redditinstance->_sendrequest("POST", "/api/unsubscribe", body.dump());
        return *this;
    }

    Subreddit & Subreddit::unsubscribe () {
        if (_redditinstance == nullptr) {
            throw NotLoggedInError("You must be logged in to unsubscribe to r/" + name + ".");
        }

        nlohmann::json body = {};
        body["action"] = "unsub";
        body["action_source"] = "a"; //required by API for unknown reason
        body["sr"] = name;
        _redditinstance->_sendrequest("POST", "/api/unsubscribe", body.dump());
        return *this;
    }

    Subreddit & Subreddit::ban (const Redditor & user, 
                                const std::string & message, 
                                int length, 
                                const std::string & reason,
                                const std::string & modnote) {
        return ban(user.username, message, length, reason, modnote);
    }

    Subreddit & Subreddit::ban (const std::string & username,
                                const std::string & message,
                                int length, 
                                const std::string & reason,
                                const std::string & modnote) {
        if (_redditinstance == nullptr) {
            throw NotLoggedInError("You must be logged in to ban a user from a subreddit.");
        }
        if (length > 999 || length < 0) {
            throw BanDurationError("A user can only be banned for either 0 (permanent) or up to 999 days.");
        }
        cpr::Payload payload = {{"api_type", "json"}, 
                                {"ban_reason", reason},
                                {"ban_message", message},
                                {"duration", std::to_string(length)},
                                {"name", username},
                                {"note", modnote},
                                {"type", "banned"}};
        try {
            _redditinstance->_sendrequest("POST", "/r/" + name + "/api/friend", payload);
        } catch (const UnauthorisedError &) {
            throw UnauthorisedError("You are not allowed to ban " + username + " from r/" + name + ".");
        } catch (const CommunicationError & error) {
            throw CommunicationError(error.what() + std::string(" while attempting to ban ") + username);
        }
        return *this;
    }

    Subreddit & Subreddit::unban (const Redditor & user) {
        return unban("", user.fullname);
    }

    Subreddit & Subreddit::unban (const std::string & username, const std::string & fullname) {
        if (_redditinstance == nullptr) {
            throw NotLoggedInError("You must be logged in to unban a user from a subreddit.");
        }
        std::string subject = fullname;
        if (fullname == "") {
            subject = _redditinstance->redditor(username).fullname;
        }
        cpr::Payload payload = {{"api_type", "json"}, {"id", subject}, {"type", "banned"}};
        try {
            _redditinstance->_sendrequest("POST", "/r/" + name + "/api/unfriend", payload);
        } catch (const UnauthorisedError &) {
            throw UnauthorisedError("You are not allowed to unban " + username + " from r/" + name + ".");
        }
        return *this;
    }
}