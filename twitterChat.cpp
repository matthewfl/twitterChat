#include <iostream>
#include <string>
#include <stdlib.h>

#include "config.h"
#include "TwitterChatBot.h"


using namespace std;

int main (int argc, char *argv[]) {
  cout << "started\n";
  TwitterChatBot twit(Twitter_UserName, Twitter_Password);
  twit.feedUrl = Twitter_Feed_url;
  cout << "starting feed\n";
  twit.startFeed();
  cout << "back\n";
  
  sleep(10);
  twit.stopFeed();
  sleep(10);
  return 0;
}
