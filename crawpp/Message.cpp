#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "crawpp/Message.h"
#include "crawpp/Redditor.h"

namespace CRAW {
    Message::Message(const nlohmann::json & data) {
        read = data["new"].get<bool>();
        subredditname = data["subreddit"].is_null() ? "" : data["subreddit"].get<std::string>();
        authorname = data["author"].get<std::string>();
        score = data["score"].get<int>();
        id = data["name"].get<std::string>();
        type = data["type"].get<std::string>();
        if (type == "unknown") {
            type = data["distinguished"].is_null() ? "pm" : "modmail";
        }
        subject = data["subject"].get<std::string>();
        body = data["body"].get<std::string>();
        fullname = data["name"].get<std::string>();
        created = data["created"].get<time_t>();
        children = std::vector<Message>();
        if (!data["replies"].is_string()) { // if there are no children, this value is "" instead of being an object
            for (auto & object : data["replies"]["data"]["children"]) {
                //recursively add children to the vector
                //this has a max depth of 1, because children can't also have children
                children.emplace_back(Message(object["data"]));
            }
        }
    }

    Redditor Message::author () {
        return Redditor(authorname, _redditinstance);
    }

    void Message::reply (const std::string & contents) {
        nlohmann::json response = _redditinstance->_sendrequest("POST", "/api/comment", cpr::Payload{{"thing_id", fullname}, {"text", contents}});
        std::string c = response.dump();
    }

    void Message::mark_read () {
        _redditinstance->_sendrequest("POST", "/api/read_message", cpr::Payload{{"id", fullname}});
    }

    void Message::mark_unread () {
        _redditinstance->_sendrequest("POST", "/api/unread_message", cpr::Payload{{"id", fullname}});
    }
}