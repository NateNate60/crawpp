#pragma once
#include <stdexcept>

namespace CRAW {
    namespace errors {
        /*
        CRAWError
        |_CommunicationError
        |   |_AuthorisationError
        |   |   |_LoginError
        |   |   |_NotLoggedInError
        |   |_NotFoundError
        |_InvalidInteractionError
        |   |_EditingError
        |   |_PostingError
        |   |_ModerationError
        |   |   |_BanError
        |   |      |_BanDurationError
        |_FileOperationError
        */


        

        /**
         * @brief The general CRAW error that all other exceptions
         * inherit from. This is the most general error
         * that CRAW classes and methods may throw.
         * 
         */
        class CRAWError : public std::runtime_error {
            public:
                CRAWError (const std::string & what) : std::runtime_error(what) {}
                CRAWError (const char * what) : std::runtime_error(what) {}
        };

        /**
         * @brief CommunicationError represents a failed attempt to communicate with the
         * server. Whenever the server either responds with something in an
         * unexpected form or an error, or something is attempted that WOULD
         * cause that, CommunicationError will be thrown.
         * 
         * This is the most generic form of retrieval error. Use more specific
         * exceptions when the cause is actually known.
         */
        class CommunicationError : public CRAWError {
            public:
                CommunicationError (const std::string & what) : CRAWError(what) {}
                CommunicationError (const char * what) : CRAWError(what) {}
        };

        /**
         * @brief Whenever the user is not authorised to
         * do something, AuthorisationError should be thrown.
         * 
         */
        class AuthorisationError : public CommunicationError {
            public:
                AuthorisationError (const std::string & what) : CommunicationError(what) {}
                AuthorisationError (const char * what) : CommunicationError(what) {}
        };

        /**
        @brief Certain features are only available if authentication information is provided, 
        e.g. making a post. If this information is not provided and the user attempts 
        to perform some action which requires authentication, NotLoggedInError will be
        thrown.
        */
        class NotLoggedInError : public AuthorisationError {
            public:
                NotLoggedInError (const std::string & what) : AuthorisationError(what) {}
                NotLoggedInError (const char * what) : AuthorisationError(what) {}
        };

        /**
        @brief An error occurred while trying to retrive the token
        */
        class LoginError : public AuthorisationError {
            public:
                LoginError (const std::string & what) : AuthorisationError(what) {}
                LoginError (const char * what) : AuthorisationError(what) {}
        };

        /**
        @brief Some tasks can only be perform with authorisation. Anything that causes the API
        to return a 403 will throw UnauthorisedError. Examples of this is trying to
        delete a post when you aren't the author or trying to remove a comment when you
        aren't a moderator.
        */
        class UnauthorisedError : public CommunicationError {
            public:
                UnauthorisedError (const std::string & what) : CommunicationError(what) {}
                UnauthorisedError (const char * what) : CommunicationError(what) {}
        };

        /**
        @brief Trying to access something that doesn't exist (or giving a monkey a shower,
        surfing tidal waves, creating nanobots, or looking in Frankenstien's brain)
        will cause NotFoundError to be thrown.
        */
        class NotFoundError : public CommunicationError {
            public:
                NotFoundError (const std::string & what) : CommunicationError(what) {}
                NotFoundError (const char * what) : CommunicationError(what) {}
        };

        /**
         * @brief Whenever the user tries to interact with something in a way that is
         * invalid, such as trying to ban themselves or edit an image post, throw
         * InvalidInteractionError.
         */
        class InvalidInteractionError : public CRAWError {
            public:
                InvalidInteractionError (const std::string & what) : CRAWError(what) {}
                InvalidInteractionError (const char * what) : CRAWError(what) {}
        };

        /**
         * @brief This error should be thrown when the user attempts to edit something
         * that can't be edited, like an image post.
        */
        class EditingError : public InvalidInteractionError {
            public:
                EditingError (const std::string & what = "") : InvalidInteractionError(what) {}
                EditingError (const char * what) : InvalidInteractionError(what) {}
        };

        /**
         * @brief When the user makes a malformed attempt to post something, PostingError is thrown
         */
        class PostingError : public InvalidInteractionError {
            public:
                PostingError (const std::string & what = "") : InvalidInteractionError(what) {}
                PostingError (const char * what) : InvalidInteractionError(what) {}
        };

        /**
         * @brief When the user makes an invalid attempt to moderate something.
         * Note that unauthorised moderation throws AuthorisationError instead.
         */
        class ModerationError : public InvalidInteractionError {
            public:
                ModerationError (const std::string & what = "") : InvalidInteractionError(what) {}
                ModerationError (const char * what) : InvalidInteractionError(what) {}
        };
        
        /**
         * @brief When the user makes an invalid attempt to ban someone, throw BanError.
         */
        class BanError : public ModerationError {
            public:
                BanError (const std::string & what = "") : ModerationError(what) {}
                BanError (const char * what) : ModerationError(what) {}
        };

        /**
         * @brief If the user attempts to ban someone for a duration fewer than 0 days or
         * more than 999 days, BanDurationError is thrown. Note that a 0-day ban
         * is a permanent ban.
         */
        class BanDurationError : public BanError {
            public:
                BanDurationError (const std::string & what = "") : BanError(what) {}
                BanDurationError (const char * what) : BanError(what) {}
        };

        /**
         * @brief FileOperationError is thrown if an attempt to open a local file for
         * reading or writing fails or would fail (such as when a malformed file path
         * is provided).
         */
        class FileOperationError : public CRAWError {
            public: 
                FileOperationError (const std::string & what = "") : CRAWError(what) {}
                FileOperationError (const char * what) : CRAWError(what) {}
        };
    }
}