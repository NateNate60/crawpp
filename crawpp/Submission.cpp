#include <nlohmann/json.hpp>
#include <string>
#include <cassert>

#include "crawpp/Reddit.h"
#include "crawpp/Comment.h"
#include "crawpp/Submission.h"
#include "crawpp/Subreddit.h"
#include "crawpp/crawexceptions.hpp"

namespace CRAW {
    Comment Submission::reply (const std::string & contents, bool distinguish) {
        if (!_redditinstance->authenticated) {
            throw errors::NotLoggedInError("You must be logged in to leave a reply.");
        }
        nlohmann::json body = {};
        body["return_rtjson"] = true;
        body["text"] = contents;
        body["thing_id"] = fullname;
        nlohmann::json response = _redditinstance->_sendrequest("POST", "/api/comment", body.dump());
        return Comment(response, _redditinstance);
    }

    Subreddit Submission::subreddit () {
        return Subreddit(subredditname, _redditinstance);
    }

    void Submission::remove (bool spam) {
        if (!_redditinstance->authenticated) {
            throw errors::NotLoggedInError("You must be logged in to remove a submission.");
        }
        nlohmann::json body = {};
        body["id"] = fullname;
        body["spam"] = spam;
        _redditinstance->_sendrequest("POST", "/api/remove", body.dump());
    }

    void Submission::del () {
        if (!_redditinstance->authenticated) {
            throw errors::NotLoggedInError("You must be logged in to delete a submission.");
        }
        nlohmann::json body = {};
        body["id"] = fullname;
        _redditinstance->_sendrequest("POST", "/api/del", body.dump());
    }

    Submission & Submission::edit (const std::string & newcontents) {
        if (!_redditinstance->authenticated) {
            throw errors::NotLoggedInError("You must be logged in to edit a submission.");
        }
        nlohmann::json body = {};
        body["id"] = fullname;
        body["return_rtjson"] = true;
        body["text"] = newcontents;
        nlohmann::json response = _redditinstance->_sendrequest("POST", "/api/editusertext", body.dump());
        if (response["code"] == 500) {
            // e.g. editing an image post
            throw errors::EditingError("You can't edit that post.");
        } else if (response["code"] == 403) {
            // e.g. editing someone else's post
            throw errors::EditingError("You aren't allowed to edit that post.");
        }
        edited = response["edited"];
        content = newcontents;

        return *this;
    }

    void Submission::_vote (int direction) {
        // 1 for upvote, -1 for downvote, 0 for clear, everything else is undefined
        assert(direction >= 1 && direction <= -1);

        if (!_redditinstance->authenticated) {
            throw errors::NotLoggedInError("You must be logged in to edit a submission.");
        }
        nlohmann::json body = {};
        body["id"] = fullname;
        body["dir"] = direction;
        _redditinstance->_sendrequest("POST", "/api/vote", body.dump());
    }

    Submission & Submission::upvote () {
        _vote(1);
        return *this;
    }

    Submission & Submission::downvote () {
        _vote(-1);
        return *this;
    }

    Submission & Submission::clearvote () {
        _vote(0);
        return *this;
    }
}