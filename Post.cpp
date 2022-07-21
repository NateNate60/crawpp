#include <nlohmann/json.hpp>
#include <restclient-cpp/connection.h>

#include "crawexceptions.hpp"
#include "Post.h"
#include "Subreddit.h"
#include "Comment.h"

using namespace CRAW;

void Post::_init (const nlohmann::json & data, const nlohmann::json & comments) {
    information = data;
    id = data["id"];
    authorname = data["author"];
    fullname = data["name"];
    title = data["title"];
    posted = static_cast<time_t>(data["created"]);
    score = static_cast<int>(data["score"]);
    flairtext = data["link_flair_text"];
    subredditname = data["subreddit"];

    if (data["post_hint"].is_null()) {
        // text posts have no hint
        type = "text";
        content = data["selftext"];
    } else {
        type = data["post_hint"];
        content = data["url"];
    }
    _comments = comments;
}

Post::Post (const std::string & id, Reddit * redditinstance) {

    // we need to use Connection rather than the simple ones because we need to follow redirects
    RestClient::Connection connection = RestClient::Connection("https://api.reddit.com");
    connection.FollowRedirects(true);
    RestClient::Response response = connection.get("/" + id);
    switch (response.code) {
        case 404:
            throw NotFoundError("No such post with ID " + id);
        case 403:
            throw UnauthorisedError("You are not authorised to view the post with ID " + id);
        case 200:
            break;
        default:
            throw CommunicationError("The server responded to an attempt to get post with ID " + id + " with error code " + std::to_string(response.code));
    }
    nlohmann::json responsejson = nlohmann::json::parse(response.body);
    if (responsejson[0]["data"].is_null()) {
        throw CommunicationError("Received a malformed response from the server when attempting to get post with ID " + id);
    }

    _init(responsejson[0]["data"], responsejson[1]["data"]["children"]);
}

Post::Post (nlohmann::json & data, Reddit * redditinstance) {
    _init(data);
}

std::vector<Comment> Post::comments (const std::string & sort, const unsigned int limit) {
    if (_comments.is_null()) {
        // we need to use Connection rather than the simple ones because we need to follow redirects
        RestClient::Connection connection = RestClient::Connection("https://api.reddit.com");
        connection.FollowRedirects(true);
        RestClient::Response response = connection.get("/" + id);
        switch (response.code) {
            case 404:
                throw NotFoundError("No such post with ID " + id);
            case 403:
                throw UnauthorisedError("You are not authorised to view the post with ID " + id);
            case 200:
                break;
            default:
                throw CommunicationError("The server responded to an attempt to get post with ID " + id + " with error code " + std::to_string(response.code));
        }
        nlohmann::json responsejson = nlohmann::json::parse(response.body)[1]["data"]["children"];
        if (responsejson.is_null()) {
            throw CommunicationError("Received a malformed response from the server when attempting to get post with ID " + id);
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

