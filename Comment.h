#pragma once
#include <nlohmann/json.hpp>

#include "Submission.h"

namespace CRAW {
    /**
    This class represents a comment on Reddit.
    @note This class doesn't have a constructor by ID because it is not
    possible to search for a comment by only its ID without knowing the ID
    of the parent post.
    */
    class Comment : public Submission {
        public:
            /**
             * Construct a new Comment object and associate it with the
             * given Reddit instance, and with the data provided.
             * @param redditinstance The Reddit instance to associate
             * @param data The "data" field of the API response for the comment, inside "children"
             */
            Comment (const nlohmann::json & data, Reddit * redditinstance = nullptr);
            
            /**
             * Fetch the replies to a comment
             * 
             * @return std::vector<Comment> of the comment's replies
             */
            std::vector<Comment> replies ();
    };
}