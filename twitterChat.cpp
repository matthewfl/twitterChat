#include <iostream>
#include <string>
#include <stdlib.h>
#include <assert.h>

#include "config.h"
#include "TwitterChatBot.h"


using namespace std;

int main (int argc, char *argv[]) {
  if(sqlite3_threadsafe() != 1) {
    cerr << "Wrong sqlite3 thread mode\n";
    sqlite3_config(SQLITE_CONFIG_SERIALIZED);
  }

  sqlite3 *dbTweet, *dbKnow;
  int rc;
  rc = sqlite3_open(Twitter_database_tweet, &dbTweet);
  rc += sqlite3_open(Twitter_database_knowledge, &dbKnow);
  if(rc) {
    cout << "opening database failed\n";
    sqlite3_close(dbTweet);
    sqlite3_close(dbKnow);
    exit(-1);
  }
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
    sleep(Twitter_Queue_checkTime);
  }
  twit.stop();
  sqlite3_close(dbTweet);
  sqlite3_close(dbKnow);
  return 0;
}
