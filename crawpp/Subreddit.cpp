#include <nlohmann/json.hpp>
#include <cpr/cpr.h>
#include <fstream>
#include <map>

#include "crawpp/Subreddit.h"
#include "crawpp/Rule.h"
#include "crawpp/Post.h"
#include "crawpp/Redditor.h"
#include "crawpp/ListingPage.hpp"
#include "crawpp/crawexceptions.hpp"

namespace CRAW {

    std::string Subreddit::_upload (const std::string & mediapath, const std::string & caption) {
        std::string mimetype;
        // gets the filename extension
        try {
            mimetype = mediapath.substr(mediapath.find_last_of("/")).substr(mediapath.find_last_of("."));
        } catch (std::exception) {
            throw errors::FileOperationError("Error while parsing file path \"" + mediapath + "\".");
        }
        // converts the filename extension to lowercase
        // https://stackoverflow.com/questions/313970/how-to-convert-an-instance-of-stdstring-to-lower-case
        std::transform(mimetype.begin(), mimetype.end(), mimetype.begin(),[](char i){return std::tolower(i);});

        // translation from filename extension to mimetype
        if (mimetype == "jpg" || mimetype == "jpeg") {
            mimetype = "image/jpeg";
        } else if (mimetype == "png") {
            mimetype = "image/png";
        } else if (mimetype == "gif") {
            mimetype = "image/gif";
        } else if (mimetype == "mov") {
            mimetype = "video/quicktime";
        } else if (mimetype == "mp4") {
            mimetype = "video/mp4";
        }

        nlohmann::json response = _redditinstance->_sendrequest("POST", "/api/media/asset.json", 
                                                                cpr::Payload{{"filepath", mediapath}, 
                                                                             {"mimetype", mimetype}});
        std::string uploadurl = "http:" + response["args"]["action"].get<std::string>();
        const nlohmann::json & fields = response["args"]["fields"];

        nlohmann::json uploaded = cpr::Post(cpr::Url(uploadurl),
                                            cpr::Multipart {
                                                {"acl", "private"},
                                                {"key", fields[1]["value"].get<std::string>()},
                                                {"X-Amz-Credential", fields[2]["value"].get<std::string>()},
                                                {"X-Amz-Algorithm", fields[3]["value"].get<std::string>()},
                                                {"X-Amz-Date", fields[4]["value"].get<std::string>()},
                                                {"success_action_status", fields[5]["value"].get<std::string>()},
                                                {"Content-Type", fields[6]["value"].get<std::string>()},
                                                {"x-amz-storage-class", fields[7]["value"].get<std::string>()},
                                                {"x-amz-meta-ext", fields[8]["value"].get<std::string>()},
                                                {"policy", fields[9]["value"].get<std::string>()},
                                                {"X-Amz-Signature", fields[10]["value"].get<std::string>()},
                                                {"x-amz-security-token", fields[11]["value"].get<std::string>()},
                                                {"file", cpr::File(mediapath)}
                                            }
                                           ).text;
        std::string imageurl = uploadurl + "/" + fields[1]["value"].get<std::string>();
        return imageurl;
    }

    Subreddit::Subreddit (const std::string & subredditname, Reddit * redditinstance) {
        try {
            information = redditinstance->_sendrequest("GET", "/r/" + subredditname + "/about")["data"];
        } catch (errors::NotFoundError &) {
            throw errors::NotFoundError("Could not find a subreddit with name r/" + subredditname);
        } catch (errors::UnauthorisedError &) {
            throw errors::UnauthorisedError("You aren't allowed to access r/" + subredditname);
        }

        if (!information["children"].is_null()) {
            // the listing having "children" means it's actually a search listing
            // and there isn't a subreddit with that exact name
            std::string similars = "";
            for (auto & i : information["children"]) {
                similars += " ";
                similars += i["data"]["display_name"];
            }
            throw errors::NotFoundError("No subreddit named \"" + subredditname + "\" exists. Did you mean any of these?" + similars);
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
                                        const int limit,
                                        ListingPage * listingpage,
                                        const std::string & direction) {
        if (limit < 0 || limit > 100) {
            throw std::invalid_argument("limit must be a number in [0, 100], not " + std::to_string(limit));
        }
        if (listingpage != nullptr && direction != "after" && direction != "before") {
            throw std::invalid_argument("The direction must be either \"after\" or \"before\", not " + direction);
        }
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
        nlohmann::json responsejson;
        try {
            cpr::Parameters parameters = {};
            if (listingpage != nullptr) {
                parameters = direction == "after" ? cpr::Parameters{{"after", listingpage->after}} : cpr::Parameters{{"before", listingpage->before}};
            }
            responsejson = _redditinstance->_sendrequest("GET", "/r/" + name + "/" + sort, {}, parameters);
        } catch (errors::UnauthorisedError &) {
            throw errors::UnauthorisedError("You don't have permission to look at r/" + name + " posts.");
        }
        if (responsejson.is_null()) {
            throw errors::CommunicationError("Malformed response from server when fetching r/" + name + " posts.");
        }

        if (listingpage != nullptr) {
            listingpage->after = responsejson["data"]["after"];
            listingpage->before = responsejson["data"]["before"];
        }

        std::vector<Post> postvector = {};

        for (auto & i : responsejson["data"]["children"]) {
            postvector.emplace_back(Post(i["data"], _redditinstance));
        }
        return postvector;


    }



    Post Subreddit::post (const std::string & title,
                          const std::string & contents,
                          const std::string & type,
                          const PostOptions & options) {
        if (type != "text" && type != "link") {
            throw std::invalid_argument("Post type must be \"text\" or \"link\", not " + type + ". To make a media post, use postmedia().");
        }
        if (!_redditinstance->authenticated) {
            throw errors::NotLoggedInError("You must be logged in to make a post.");
        }
        if (options.event_start == 0 && options.event_end != 0 || options.event_start != 0 && options.event_end == 0) {
            throw errors::PostingError("Either both event_start and event_end must be specified, or neither, but not one.");
        }
        if (options.event_start != 0 && options.event_timezone == "") {
            throw errors::PostingError("A time zone was not specified.");
        }
        cpr::Payload payload = {{"title", title}, 
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
        if (options.event_start != 0) {

            // It's only a maximum of 20 characters long but better safe than sorry
            // buffer overflow begone!!
            char startstring [25];
            char endstring [25];

            // format the string to the desired format, which is YYYY-MM-DDTHH:MM:SS
            strftime(startstring, 24, "%FT%T", localtime(&options.event_start));
            strftime(endstring, 24, "%FT%T", localtime(&options.event_end));

            payload.Add({"event_start", startstring});
            payload.Add({"event_end", endstring});
        }

        if (type == "text") {
            payload.Add({"kind", "self"});
        } else {
            payload.Add({"kind", "link"});
            payload.Add({"url", contents});
        }

        nlohmann::json response = _redditinstance->_sendrequest("POST", "/api/submit", payload);
        if (!response["status"].is_null()) {
            throw errors::CommunicationError("The server gave a malformed response while attempting to make a post.");
        }
        if (!response["success"]) {

            //server responds with a jquery object
            std::string error = response["jquery"][10][3][0];
            std::string description = response["jquery"][14][3][0];

            throw errors::PostingError("The following error was encountered when attempting to post: \"" +
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

    void Subreddit::postmedia (const std::string & title,
                               const std::string & contents,
                               const std::string & type,
                               const PostOptions & options) {
        if (!_redditinstance->authenticated) {
            throw errors::NotLoggedInError("You must be logged in to make a post.");
        }
        if (options.event_start == 0 && options.event_end != 0 || options.event_start != 0 && options.event_end == 0) {
            throw errors::PostingError("Either both event_start and event_end must be specified, or neither, but not one.");
        }
        if (options.event_start != 0 && options.event_timezone == "") {
            throw errors::PostingError("A time zone was not specified.");
        }
        cpr::Payload payload = {{"title", title}, 
                                {"api_type", "json"},
                                {"show_error_list", "true"},
                                {"original_content", options.oc ? "true" : "false"},
                                {"validate_on_submit", "true"},
                                {"nsfw", options.nsfw ? "true" : "false"},
                                {"sendreplies", options.get_inbox_replies ? "true" : "false"},
                                {"spoiler", options.spoiler ? "true" : "false"},
                                {"post_to_twitter", "false"},
                                {"sr", name},
                                {"submit_type", "subreddit"}};
        if (options.collection_id != "") {
            payload.Add({"collection_id", options.collection_id});
        }
        if (options.flair_id != "") {
            payload.Add({"flair_id", options.flair_id});
        }
        if (options.event_start != 0) {

            // It's only a maximum of 20 characters long but better safe than sorry
            // buffer overflow begone!!
            char startstring [25];
            char endstring [25];

            // format the string to the desired format, which is YYYY-MM-DDTHH:MM:SS
            strftime(startstring, 24, "%FT%T", localtime(&options.event_start));
            strftime(endstring, 24, "%FT%T", localtime(&options.event_end));

            payload.Add({"event_start", startstring});
            payload.Add({"event_end", endstring});
        }

        if (type == "image") {
            payload.Add({"kind", "image"});
            payload.Add({"url", _upload(contents)});
        } else if (type == "video") {
            payload.Add({"kind", "video"});
            payload.Add({"url", _upload(contents)});
            payload.Add({"video_poster_url", _upload("./blank.png")});
        } else {
            throw std::invalid_argument("Post type must be \"image\" or \"video\", not " + type + ". To make a text post, use post().");
        }

        nlohmann::json response = _redditinstance->_sendrequest("POST", "/api/submit", payload);
        if (!response["status"].is_null()) {
            throw errors::CommunicationError("The server gave a malformed response while attempting to make a post.");
        }
        if (!response["success"]) {

            //server responds with a jquery object
            std::string error = response["jquery"][10][3][0];
            std::string description = response["jquery"][14][3][0];

            if (description == "you should check that url") {
                // uploaded a bad image/nonexistent file
                throw errors::PostingError("Failed to upload " + contents + ".");
            }

            throw errors::PostingError("The following error was encountered when attempting to post: \"" +
                            error +
                            "\" - \"" +
                            description +
                            "\"");
        }
    }

    Subreddit & Subreddit::subscribe (bool skip_initial_defaults) {
        if (!_redditinstance->authenticated) {
            throw errors::NotLoggedInError("You must be logged in to subscribe to r/" + name + ".");
        }

        cpr::Payload body = {{"action", "sub"},
                             {"action_source", "a"}, //required by API for unknown reason
                             {"skip_initial_defailts", skip_initial_defaults ? "true" : "false"},
                             {"sr", fullname}};
        _redditinstance->_sendrequest("POST", "/api/subscribe", body);
        return *this;
    }

    Subreddit & Subreddit::unsubscribe () {
        if (!_redditinstance->authenticated) {
            throw errors::NotLoggedInError("You must be logged in to unsubscribe to r/" + name + ".");
        }

        cpr::Payload body = {{"action", "unsub"},
                             {"action_source", "a"}, //required by API for unknown reason
                             {"sr", fullname}};
        _redditinstance->_sendrequest("POST", "/api/subscribe", body);
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
        if (!_redditinstance->authenticated) {
            throw errors::NotLoggedInError("You must be logged in to ban a user from a subreddit.");
        }
        if (length > 999 || length < 0) {
            throw errors::BanDurationError("A user can only be banned for either 0 (permanent) or up to 999 days.");
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
        } catch (const errors::UnauthorisedError &) {
            throw errors::UnauthorisedError("You are not allowed to ban " + username + " from r/" + name + ".");
        } catch (const errors::CommunicationError & error) {
            throw errors::CommunicationError(error.what() + std::string(" while attempting to ban ") + username);
        }
        return *this;
    }

    Subreddit & Subreddit::unban (const Redditor & user) {
        return unban("", user.fullname);
    }

    Subreddit & Subreddit::unban (const std::string & username, const std::string & fullname) {
        if (!_redditinstance->authenticated) {
            throw errors::NotLoggedInError("You must be logged in to unban a user from a subreddit.");
        }
        std::string subject = fullname;
        if (fullname == "") {
            subject = _redditinstance->redditor(username).fullname;
        }
        cpr::Payload payload = {{"api_type", "json"}, {"id", subject}, {"type", "banned"}};
        try {
            _redditinstance->_sendrequest("POST", "/r/" + name + "/api/unfriend", payload);
        } catch (const errors::UnauthorisedError &) {
            throw errors::UnauthorisedError("You are not allowed to unban " + username + " from r/" + name + ".");
        }
        return *this;
    }

    std::vector<Rule> Subreddit::rules () {
        nlohmann::json response = _redditinstance->_sendrequest("GET", "/r/" + name + "/about/rules");
        std::vector<Rule> rules;
        for (auto & i : response["rules"]) {
            Rule rule;
            rule.kind = i["kind"].get<std::string>();
            rule.description = i["description"].get<std::string>();
            rule.name = i["short_name"].get<std::string>();
            rule.report_reason = i["violation_reason"].get<std::string>();
            rule.created = i["created_utc"].get<time_t>();
            rule.number = i["priority"].get<int>();
            rules.emplace_back(rule);
        }
        return rules;
    }
}