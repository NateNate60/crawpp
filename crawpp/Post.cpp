#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

#include "crawpp/crawexceptions.hpp"
#include "crawpp/Post.h"
#include "crawpp/Subreddit.h"
#include "crawpp/Comment.h"

namespace CRAW {

    void Post::_init (const nlohmann::json & data, const nlohmann::json & comments) {
        information = data;
        id = data["id"].get<std::string>();
        authorname = data["author"].get<std::string>();
        fullname = data["name"].get<std::string>();
        title = data["title"].get<std::string>();
        posted = data["created"].get<time_t>();
        score = data["score"].get<int>();
        flairtext = data["link_flair_text"].get<std::string>();
        subredditname = data["subreddit"].get<std::string>();
        if (data["edited"].is_number()) {
            edited = data["edited"].get<time_t>();
        } else {
            edited = 0;
        }

        if (data["post_hint"].is_null()) {
            // text posts have no hint
            type = "text";
            content = data["selftext"].get<std::string>();
        } else {
            type = data["post_hint"].get<std::string>();
            content = data["url"].get<std::string>();
        }
        _comments = comments;

        awards = std::set<Award> ();
        for (auto & i : data["all_awardings"]) {
            awards.insert(Award(i));
        }
    }

    Post::Post (const std::string & id, Reddit * redditinstance) {
        nlohmann::json responsejson;
        try {
            responsejson = redditinstance->_sendrequest("GET", "https://api.reddit.com/" + id);
        } catch (errors::NotFoundError &) {
            throw errors::NotFoundError("No such post with ID " + id);
        } catch (errors::UnauthorisedError &) {
            throw errors::UnauthorisedError("You are not authorised to view the post with ID " + id);
        }

        if (responsejson[0]["data"].is_null()) {
            throw errors::CommunicationError("Received a malformed response from the server when attempting to get post with ID " + id);
        }

        _init(responsejson[0]["data"], responsejson[1]["data"]["children"]);
    }

    Post::Post (nlohmann::json & data, Reddit * redditinstance) {
        _init(data);
    }

    std::vector<Comment> Post::comments (const std::string & sort, const unsigned int limit) {
        if (_comments.is_null()) {
            nlohmann::json responsejson;
            try {
                responsejson = _redditinstance->_sendrequest("GET", "/" + id)[1]["data"]["children"];
            } catch (errors::NotFoundError &) {
                throw errors::NotFoundError("No such post with ID " + id);
            } catch (errors::UnauthorisedError &) {
                throw errors::UnauthorisedError("You are not authorised to view the post with ID " + id);
            }

            if (responsejson.is_null()) {
                throw errors::CommunicationError("Received a malformed response from the server when attempting to get post with ID " + id);
            }
            _comments = responsejson;
        }
        std::vector<Comment> commentvector = {};
        for (auto & i : _comments) {
            commentvector.push_back(Comment(i["data"], _redditinstance));
        }
        // note that the returning by value is actually not that slow because of RVO
        return commentvector;
    }

}