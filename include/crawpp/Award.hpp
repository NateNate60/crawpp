#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <ctime>

namespace CRAW {

    /**
     * @brief A struct representing a type of award that is given
     * to a post.
     * 
     * Common attributes are available as members, however, all
     * information returned by the API is available in the 
     * information member as a JSON object, and can be accessed
     * using the [] operator (e.g. Award.information["is_new"]).
     */
    struct Award {
        /// All information about the award returned by the API
        nlohmann::json information;

        /// The price (in coins) of the award
        int price;

        /// The award's ID
        std::string id;

        /// The fullname of the subreddit that this award is exclusive to (empty if none)
        std::string subreddit_fullname;

        /// The URL of the icon
        std::string icon;

        /// The number of days of Reddit Premium granted by the award
        int premium_days;

        /// Whether the award is enabled
        bool enabled;

        /// The award's description
        std::string description;

        /// The number of coins this award gives to the subreddit
        int subreddit_coins;

        /// The number of awards of this kind that this struct represents
        /// If multiple duplicate awards are given, then instead of appearing
        /// multiple times, this number will be set.
        int count;

        /// The name of the award
        std::string name;

        /**
         * @brief Construct a new Award object with the given data
         * 
         * @param data A JSON object containing the data about the award
         */
        Award (const nlohmann::json & data) {
                count = data["count"].get<int>();
                description = data["description"].get<std::string>();
                enabled = data["is_enabled"].get<bool>();
                icon = data["icon_url"].get<std::string>();
                id = data["id"].get<std::string>();
                information = data;
                name = data["name"].get<std::string>();
                premium_days = data["days_of_premium"].get<int>();
                price = data["price"].get<int>();
                subreddit_coins = data["subreddit_coin_reward"].get<int>();
                subreddit_fullname = data["subreddit_id"].get<std::string>();
        }

        /**
         * @brief Two Award objects are equal if and only if they have the
         * same name.
         * 
         * @return true a.name == b.name
         * @return false a.name != b.name
         */
        bool operator== (const Award & b) {
            return b.name == name;
        }
    };

}