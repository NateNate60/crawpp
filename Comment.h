#pragma once
#include <ctime>

#include "Reddit.h"

namespace CRAW {
    /**
    This class represents a comment on Reddit.
    Note: This class doesn't have a constructor (other than the default) because
    it is not possible to search for a comment by only its ID without knowing the ID
    of the parent post.
    */
    class Comment {
        private:
            const Reddit * _redditinstance;
        public:
            // The content of the comment
            std::string content;

            // The comment's current score. Note that due to vote-fuzzing this is not 100% accurate.
            int score;

            // The username of the comment's author
            std::string authorname;
            // Fetches a Redditor instance for the author of the post
            Redditor author ();

            // The UTC time at which the comment was posted
            time_t posted;

            // Returns whether two comments are the same (i.e. have the same ID)
            bool operator== (const Comment & comment);

            /**
            Reply to a comment. Returns the new comment.

            @param contents: The Markdown contents of the reply
            @param distinguish: Whether to distinguish as a moderator
            @return The Comment that was left as a reply
            */
            Comment reply (std::string contents, bool distinguish = false); 

            /**
            Remove the comment (as a moderator).

            @param spam: Whether to mark the comment as spam, which trains the spam filter
            */
            void remove (bool spam = false);

            /**
            Delete the comment (as the author)
            This would be called "delete" if that weren't a reserved keyword in C++.
            */
            void deletecomment ();

            /**
            Edit the comment (as the author). Returns a reference to the comment so
            calls can be chained together (e.g. comment.reply().remove()).

            @param newcontent: The new contents to replace the current comment contents
            @return A reference to the Comment that was edited
            */
            Comment & edit (const std::string & newcontents);

            /**
            Upvote a comment. Returns a reference to the comment so
            calls can be chained together (e.g. comment.reply().remove()).

            Note: A human must to be the one triggering this action. Reddit prohibits 
            automated voting by bots as that is considered vote-cheating.

            @return A reference to the Comment that was upvoted
            */
            Comment & upvote ();

            /**
            Downvote a comment. Returns a reference to the comment so
            calls can be chained together (e.g. comment.reply().remove()).

            Note: A human must to be the one triggering this action. Reddit prohibits 
            automated voting by bots as that is considered vote-cheating.

            @return A reference to the Comment that was downvoted
            */
            Comment & downvote ();

    };
}