libcrawpp.a: Reddit.o Redditor.o Subreddit.o Post.o Comment.o
	ar crf libcrawpp.a Reddit.o Redditor.o Subreddit.o Post.o Comment.o

Reddit.o: Reddit.cpp Reddit.h
	g++ -g -c Reddit.cpp -lcpr --std=c++17

Redditor.o: Redditor.cpp Redditor.h
	g++ -g -c Redditor.cpp -lcpr --std=c++17

Subreddit.o: Subreddit.cpp Subreddit.h
	g++ -g -c Subreddit.cpp -lcpr --std=c++17

Post.o: Post.cpp Post.h
	g++ -g -c Post.cpp -lcpr --std=c++17

Comment.o: Comment.cpp Comment.h
	g++ -g -c Comment.cpp -lcpr --std=c++17

a.out: test.cpp libcrawpp.a
	g++ -g -I. -L. test.cpp -lcrawpp -lcpr --std=c++17

install: libcrawpp.a Reddit.h Redditor.h Subreddit.h Post.h Comment.h
	cp libcrawpp.a /usr/local/lib
	mkdir /usr/local/include/crawpp
	cp Reddit.h Redditor.h Subreddit.h Post.h Comment.h /usr/local/include/crawpp
	ldconfig
	echo "Done"

remove:
	rm /usr/local/include/crawpp -r
	rm /usr/local/lib/libcrawpp.a
	ldconfig
	echo "Done"