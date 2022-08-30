#include <nlohmann/json.hpp>

#include "crawpp/Reddit.h"
#include "crawpp/Comment.h"
#include "crawpp/Award.hpp"

namespace CRAW {
    Comment::Comment (const nlohmann::json & data, Reddit * redditinstance) {
        information = data;
        id = data["id"].get<std::string>();
        authorname = data["author"].get<std::string>();
        fullname = data["name"].get<std::string>();
        posted = data["created"].get<int>();
        score = data["score"].get<int>();
        subredditname = data["subreddit"].get<std::string>();
        type = "comment";
        if (data["edited"].is_number()) {
            edited = data["edited"].get<time_t>();
        } else {
            edited = 0;
        }
        depth = data["depth"].get<int>();
        content = data["body"].get<std::string>();

        awards = std::vector<Award> ();
        for (auto & i : data["all_awardings"]) {
            awards.emplace_back(Award(i));
        }
    }

    std::vector<Comment> Comment::replies () {
        std::vector<Comment> replylist = {};
        for (auto & i : information["replies"]["data"]["children"]) {
            replylist.emplace_back(Comment(i["data"], _redditinstance));
        }
        return replylist;
    }
}