#pragma once
#include <string>

#include "Redditor.h"
#include "Subreddit.h"
namespace CRAW {

    /**
    Represents the user's session with Reddit.
    */
    class Reddit {
        private:
            std::string _password;
            std::string _apisecret;
            static unsigned int _instancecount;
        public:
            // True if authetication info was provided, false if not
            bool authenticated;

            // The username of the Reddit account associated with the current session, empty if not authenticated
            std::string username;

            // The user agent string associated with the current session, empty if not authenticated
            std::string useragent;

            // The user id associated with the current session, empty if not authenticated
            std::string userid;

            /**
            Passing arguments to the initialiser indicates authenticating (logging in) with Reddit. The credentials are not checked until the first request is made.
            
            @param user_name: The username of the account to authenticate with
            @param password: The password of the account to log in with
            @param user_id: The user id of the API key to log in with
            @param api_secret: The API secret of the key to log in with
            @param user_agent: Any string except empty. It will be used as the user agent and associated with the current session.
            */
            Reddit (const std::string & user_name, 
                    const std::string & password, 
                    const std::string & user_id, 
                    const std::string & api_secret, 
                    const std::string & user_agent);

            // Passing no arguments to the initialiser indicates not authenticating with Reddit
            Reddit ();

            ~Reddit ();

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
    };

    unsigned int Reddit::_instancecount = 0;
}