libcrawpp.a: Reddit.o Redditor.o Subreddit.o Post.o Comment.o
	ar crf libcrawpp.a Reddit.o Redditor.o Subreddit.o Post.o Comment.o

Reddit.o: Reddit.cpp Reddit.h
	g++ -g -c Reddit.cpp -lcpr

Redditor.o: Redditor.cpp Redditor.h
	g++ -g -c Redditor.cpp -lcpr

Subreddit.o: Subreddit.cpp Subreddit.h
	g++ -g -c Subreddit.cpp -lcpr

Post.o: Post.cpp Post.h
	g++ -g -c Post.cpp -lcpr

Comment.o: Comment.cpp Comment.h
	g++ -g -c Comment.cpp -lcpr

a.out: test.cpp libcrawpp.a
	g++ -g -I. -L. test.cpp -lcrawpp -lcpr

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