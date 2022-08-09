#pragma once

#include <string>
#include <ctime>

namespace CRAW {
    /**
     * @brief A structure representing a subreddit rule.
     * 
     */
    struct Rule {
        /// The type of posts that this rule applies to
        std::string kind;

        /// The description of the rule
        std::string description;

        /// The name (title) of the rule
        std::string name;

        /// The report reason that can be used to report submissions for violating this rule
        std::string report_reason;

        /// The time (UTC) that this rule was made
        time_t created;

        /// The order of the rule in the rules list
        int number;
    };
}