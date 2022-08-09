#pragma once

#include <set>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <string>
#include <ctime>

#include "crawpp/CRAWObject.h"

namespace CRAW {
    // Forward-declarations of classes to avoid having header files #include each other
    class Redditor;
    class Submission;
    class Subreddit;
    class Post;
    class Comment;


    /**
    @brief Represents the user's session with Reddit.
    */
    class Reddit : public CRAWObject{
        private:
            std::string _password;
            std::string _apisecret;

            /**
			Stores the API token
			*/
            std::string _token;
            /**
			Stores the API token's expiration time
			*/
            time_t _expiration;

            /**
             * Get a new API token using the authentication data
             */
            void _gettoken ();

            /**
             * Send a request to the Reddit API
             * 
             * @param method The HTTP method to use (e.g. "POST", "GET")
             * @param targeturl The target URL (e.g. "/api/v1/me")
             * @param body The data to be sent in the body of the request
             * @return JSON object representing the server's response
             */
            nlohmann::json _sendrequest (const std::string & method, 
                                         const std::string & targeturl, 
                                         const std::string & body = "");

            /**
             * Send a request to the Reddit API
             * 
             * @param method The HTTP method to use (e.g. "POST", "GET")
             * @param targeturl The target URL (e.g. "/api/v1/me")
             * @param body The data to be sent in the body of the request
             * @param parameters The parameters of the request
             * @return JSON object representing the server's response
             */
            nlohmann::json _sendrequest (const std::string & method, 
                                         const std::string & targeturl, 
                                         const cpr::Payload & body,
                                         const cpr::Parameters & parameters = {});

            // All classes that can post to the API are friends
            // All classes that teach mathematics are enemies
            friend class Redditor;
            friend class Submission;
            friend class Subreddit;
        public:
            /**
			Whether the session is authenticated
			*/
            bool authenticated;

            /**
			The username of the Reddit account associated with the current session, empty if not authenticated
			*/
            std::string username;

            /**
			The user agent string associated with the current session, empty if not authenticated
			*/
            std::string useragent;

            /**
			The user id associated with the current session, empty if not authenticated
			*/
            std::string clientid;

            /**
            Passing arguments to the initialiser indicates authenticating (logging in) with Reddit. The credentials are not checked until the first request is made.
            
            @param user_name: The username of the account to authenticate with
            @param password: The password of the account to log in with
            @param client_id: The user id of the API key to log in with
            @param api_secret: The API secret of the key to log in with
            @param user_agent: Any string except empty. It will be used as the user agent and associated with the current session.
            */
            Reddit (const std::string & user_name, 
                    const std::string & password, 
                    const std::string & client_id, 
                    const std::string & api_secret, 
                    const std::string & user_agent);

            /**
			Passing no arguments to the initialiser indicates not authenticating with Reddit
			*/
            Reddit ();

            /**
            Returns a Redditor instance of the current user.
            */
            Redditor me ();
            
            /**
            Fetches a Redditor instance for the Redditor with the given username (without the u/)

            @param name: The name of the Redditor without the u/
            @return Redditor instance for the given Redditor
            */
            Redditor redditor (const std::string & name);

            /**
            Fetches a Subreddit instance for the subreddit with the given name (without the r/)
            
            @param name: The name of the subreddit without the r/
            @return Subreddit instance of the given subreddit
            */
            Subreddit subreddit (const std::string & name);

            /**
            Fetches a Post instance for the Reddit post with the given ID

            @param id: The ID of the post
            @return Post instance of the given post
            */
            Post post (const std::string & id);

            /**
             * @brief Search for subreddits that begin with a given string.
             * 
             * @param query A phrase to search for
             * @param exact When set to true, only exact matches will be returned (default: false)
             * @param nsfw Whether to include subreddits marked as NSFW in the search (default:
             * false)
             * @param autocomplete Whether to use the subreddit_autocoomplete endpoint, which
             * can automatically correct typographical errors and returns fuzzy matches (default:
             * true).
             * @param limit The number of results to return (ignored if autocomplete is false)
             * @return std::multiset<std::string> of subreddit names which begin with the query
             */
            std::multiset<std::string> search (const std::string & query, bool exact = false, bool nsfw = false, bool autocomplete = true, int limit = 5);
    };
}