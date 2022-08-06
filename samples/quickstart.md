# Quick Start

It's easy to get started with CRAW++. Many of the "scarier" features of C/C++ are entirely optional. For example, it is completely unnecessary to use `new` and `delete`, and pointers are also unnecessary.

This code snippet makes a post on r/test with "This is the title!" as the title and "Hello world!" as the body. It then subscribes to r/test, makes a comment, deletes the comment, upvotes the post, and then unsubscribes from r/test:

```cpp
// everything you need is in craw.h
#include <crawpp/craw.h>

#include <iostream>

int main () {
    CRAW::Reddit reddit = CRAW::Reddit("username", 
                                       "SUpeR_S3cuRe-Pa$$w0rd",
                                       "client_id",
                                       "api_secret",
                                       "MyBotUserAgent/1.0");
    // go to r/test
    CRAW::Subreddit subreddit = reddit.subreddit("test");
    std::cout << "r/test has " << subreddit.subscribers << "subscribers" << std::endl;
    subreddit.subscribe();
    CRAW::Post post = subreddit.post("This is the title!", "Hello world!");
    CRAW::Comment comment = post.reply("Wow, this is a neat post! This is my comment!");
    comment.del();
    post.upvote();
    subreddit.unsubscribe();
}
```

All models are in the `CRAW` namespace.

## The `Reddit` Class

The `Reddit` class is your gateway to the Reddit API. Most interacts with Reddit directly that are not tied to some other object will be done by calling the member functions of the `Reddit` class. Logging into Reddit is performed by providing login credentials when initialising the class.

```cpp
CRAW::Reddit reddit = CRAW::Reddit("username", 
                                   "SUpeR_S3cuRe-Pa$$w0rd",
                                   "client_id",
                                   "api_secret",
                                   "MyBotUserAgent/1.0");
// Logging in is optional, but most things don't work if you are not logged in
CRAW::Reddit unauthenticated CRAW::Reddit();
```

## CRAW++ Classes

All familiar Reddit objects are modelled using convenient classes, such as `Post`, `Subreddit`, and `Redditor`. These represent exactly what you'd expect them to. However, to get instances of these classes you should class the `Reddit` object's member functions. You also can, but shouldn't, initialise them directly using their constructor. This is the recommended way to get a `Subreddit` instance representing r/gaming, a `Redditor` instance representing u/NateNate60, and a `Post` instance representing [this post](https://reddit.com/r/pics/comments/92dd8/test_post_please_ignore/):

```cpp
CRAW::Subreddit r_gaming = reddit.subreddit("gaming");
CRAW::Redditor nate = reddit.redditor("NateNate60");

// note that for posts, you need the ID of the post
CRAW::Post some_post = reddit.post("92dd8");
```

It's not possible to fetch a given *comment* by ID because the Reddit API doesn't offer a good way to do that.

Each class has convenient methods to do everything you would expect to be able to do on the actual Reddit site or app.

```cpp
//gib updoot
some_post.upvote()
//...but note that automatic voting by bots is not allowed
// bots can only proxy single votes by humans

std::cout << nate.totalkarma << std::endl;
nate.follow();

r_gaming.subscribe();
```

That's all there is to it! Manual memory management is not needed unless you chose to use `new`. If you did, don't forget to `delete` when you're done. Take a look at the [Classes section](https://natenate60.xyz/crawpp/annotated.html) to see the full description for each class's methods and members.