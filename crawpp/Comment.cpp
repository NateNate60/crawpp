#include <nlohmann/json.hpp>

#include "crawpp/Reddit.h"
#include "crawpp/Comment.h"

namespace CRAW {
    Comment::Comment (const nlohmann::json & data, Reddit * redditinstance) {
        information = data;
        id = data["id"];
        authorname = data["author"];
        fullname = data["name"];
        posted = static_cast<time_t>(data["created"]);
        score = data["score"];
        subredditname = data["subreddit"];
        type = "comment";
        if (data["edited"].is_number()) {
            edited = data["edited"];
        } else {
            edited = 0;
        }
        depth = data["depth"];
        content = data["body"];
    }

    std::vector<Comment> Comment::replies () {
        std::vector<Comment> replylist = {};
        for (auto & i : information["replies"]["data"]["children"]) {
            replylist.push_back(Comment(i["data"], _redditinstance));
        }
        return replylist;
    }
}