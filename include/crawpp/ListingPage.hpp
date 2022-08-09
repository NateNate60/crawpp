#pragma once

#include <string>

namespace CRAW {
    /**
     * @brief A structure used to flip forward/backwards through
     * pages of listings.
     * 
     * @warning Don't edit this struct other than initialising it.
     * Only CRAW++ class methods should modify this struct.
     */
    struct ListingPage {

        /// The page that comes after this one
        std::string after;

        /// The page that comes before this one
        std::string before;

        ListingPage () {
            after = "";
            before = "";
        }
    };
}