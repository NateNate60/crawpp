#pragma once

#include <string>

namespace CRAW {
    /**
     * @brief A structure used to flip forward/backwards through
     * pages of listings.
     * 
     */
    struct ListingPage {
        std::string after;
        std::string before;

        ListingPage () {
            after = "";
            before = "";
        }
    };
}