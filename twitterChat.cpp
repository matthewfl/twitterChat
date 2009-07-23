#include <iostream>
#include <string>
#include <stdlib.h>
#include <assert.h>

#include "config.h"
#include "TwitterChatBot.h"


using namespace std;

int main (int argc, char *argv[]) {
  sqlite3 *dbTweet, *dbKnow;
  sqlite3_open(Twitter_database_tweet, &dbTweet);
  sqlite3_open(Twitter_database_knowledge, &dbKnow);
#ifdef Twitter_login_args
  assert(argc > 1);
  TwitterChatBot twit(argv[1], dbTweet, dbKnow);
#else
  TwitterChatBot twit(Twitter_UserName, Twitter_Password, dbTweet, dbKnow);
#endif
  twit.feedUrl = Twitter_Feed_url;
  cout << "starting feed\n";
  twit.startFeed();

  
  while(1) {
    if(twit.queueSize() > Twitter_Queue_maxsize) {
      twit.stopFeed();
    }else{
      twit.startFeed();
    }
    sleep(5);
  }
  twit.stopFeed();
  //sleep(10);
  return 0;
}
