CRAW++
======

CRAW++ is the _C++ Reddit API Wrapper_. CRAW++ is intended to be used as a library which can be included in a C++ project to access the Reddit API rather than making HTTP calls manually using `libcurl` or some other library. CRAW++ strives to be as easy to learn and use as possible while still being feature-rich. This project exists mainly because previous attempts to create a C++ Reddit API wrapper were unmaintained and undocumented.

This project is unfinished.

## Dependencies

This project depends on the following libraries:

- `libcurl` (install `libcurl4-openssl-dev` on Ubuntu/Debian)
- [nlohmann/json](https://github.com/nlohmann/json) (`nlohmann-json3-dev`)
- [libcpr/cpr](https://github.com/libcpr/cpr/)

## Installation

The makefile only works with **GCC**. Any recent version of GCC will work. It must support C++17.

### Linux (recommended)

**Build from source (recommended)**
1. Install the dependencies. `libcpr` can be installed off GitHub using `cmake` or using `vcpkg`; see [libcpr/cpr](https://github.com/libcpr/cpr/) for instructions. `libcurl-openssl-dev` and `nlohmann-json3-dev` should be included in your distribution's repositories.
2. Run `make` and `make install`. The Makefile will also run `ldconfig` for convenience.

**Precompiled library**

A `.deb` package is available for 64-bit Debian and Debian-based operating systems (such as Ubuntu). Simply install this package in the usual `apt install` fashion.

Note that this package does not install `libcpr` because `libcpr` is not found in Debian or Ubuntu's default repositories. Please follow the directions at [libcpr/cpr](https://github.com/libcpr/cpr/) to install `libcpr`.

**Making the `.deb` package**

The `.deb` package can be made using the Makefile with `make debpackage`. This makes a package in the root directory that can be installed with `dpkg`.

### Windows

In general, it is far easier to use [Windows Subsystem for Linux](https://en.wikipedia.org/wiki/Windows_Subsystem_for_Linux) and develop in the Linux environment it provides than to attempt to set this up natively on Windows. It is also less prone to failure.

1. Install G++. Follow [Microsoft's instructions to install MingGW](https://code.visualstudio.com/docs/cpp/config-mingw). Only the "prerequisites" section needs to be followed.Steps 1 and 2 can be skipped if you don't intend to use VS Code.
2. Install `make`. You can do this through [Chocolatey](https://community.chocolatey.org/packages/make), the Windows package manager with `choco install make`, or use the "MSYS2 MSYS" program install with MinGW and run `pacman -S make`.
3. Install dependencies. `cpr` and `nlohman-json` can be installed using `vcpkg`. `vcpkg` requires a compatible Visual Studio (not VS Code) instance to be install on your Windows PC. The Community edition of Visual Studio, which is free of charge, works for this. You do not have to *use* Visual Studio, but `vcpkg` requires a file provided by it to install the dependencies.
4. Within your `vcpkg` folder, add `\packages\nlohmann-json_x86-windows\include` and `vcpkg\packages\cpr_x86-windows\include` to your include path
5. Compile the static library using `make`. Note that `make install` does not work on Windows due to Windows having a different file structure.
6. Copy the static binary and the contents of the `include` folder to your working directory.

### macOS

macOS is unsupported because I don't have a Mac to test this library with.

## Usage

Documentation is available at  https://natenate60.xyz/crawpp. Please refer to the examples to get a rough idea of what this library can do.

Please note that all clients are required to authenticate using OAuth, and currently the only supported way to do this is to fetch API keys on your Reddit account. You must [get API keys](https://github.com/reddit-archive/reddit/wiki/OAuth2) and then pass them to a `Reddit` instance.

Here's a simple example which logs into Reddit and then makes a hello world post to r/test:

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

Compile this program like this:

```bash
g++ myprogram.cpp -lcrawpp -lcpr -o myprogram
```

Just remember to always tell the linker to link `libcrawpp` and `libcpr`.