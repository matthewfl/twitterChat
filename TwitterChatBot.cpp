#include "TwitterChatBot.h"
#include <pthread.h>
#include <curl/curl.h>
#include <matthewfl/json.hpp>

#include <string>
#include <stdlib.h>
#include <iostream>
using namespace std;

void TwitterChatBot::startFeed () {
  /// if feed is running do nothing
  if(!feedRunning) {
    feedRunning = true;
    pthread_create(&Pfeed, NULL, StreamThreadStart, (void*)this);
  }
}

void TwitterChatBot::stopFeed() {
  if(feedRunning) {
    pthread_cancel(Pfeed);
    curl_easy_pause(curl, CURLPAUSE_ALL);
    curl_easy_cleanup(curl);
    curl = NULL;
    feedRunning = false;
  }
}

void * TwitterChatBot::StreamThreadStart(void * self) {
  TwitterChatBot * s = reinterpret_cast<TwitterChatBot*>(self);
  // open the socket
  cout<<"here\n";
  CURL * curl = curl_easy_init();
  s->curl = curl;
  CURLcode result;
  // login in info
  curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
  curl_easy_setopt(curl, CURLOPT_USERPWD, s->loginInfo.c_str());

  // other the url to use
  curl_easy_setopt(curl, CURLOPT_URL, s->feedUrl.c_str());

  // other info
  curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0 matthewfl.com-chatBot/1.0");
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
  curl_easy_setopt(curl, CURLOPT_HEADER, 0);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, s);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, StreamCallBack);

  cout << "preform\n";
  result = curl_easy_perform(curl);

  // thread done running
  s->feedRunning = false;

  // close the socket
  curl_easy_cleanup(curl);
  s->curl = NULL;

  return NULL; // what am I to return here?

}

size_t TwitterChatBot::StreamCallBack (char * str, size_t size, size_t nmemb, TwitterChatBot * self) {
  string s;
  s.append(str, size * nmemb);
  self->tempHolder += s;
  if(self->tempHolder.find("\n") != string::npos) {
    self->proccessStream(self->tempHolder);
    self->tempHolder="";
  }
  return size * nmemb;
}

void TwitterChatBot::proccessStream (string & data) {
  //cout << data << endl << endl;
  matthewfl::json j;
  j.prase(data);
  if(!j["delete"].empty()) {
    // TODO: make a delete spool for deleting tweets out of the data base
    return;
  }
  if(!j["text"].empty()) {
     cout << j["text"].cast<matthewfl::json::String>() << endl;
    if(!j["in_reply_to_user_id"].empty()) {
      cout << "reply to: ";
      cout << j["in_reply_to_user_id"].cast<matthewfl::json::Number>() << endl << endl;
    }
  }
}

void * TwitterChatBot::AddTweetData (void * s) {
  TwitterChatBot * self = reinterpret_cast<TwitterChatBot*>(s);
  while(1) {
    while(!self->tweetQueue.empty()) {
      
    }
    sleep(1);
  }
  return NULL;
}

unsigned int TwitterChatBot::queueSize() {
  return tweetQueue.size();
}

void TwitterChatBot::start() {
  pthread_create(&PtweetData, NULL, AddTweetData, (void*)this);
}

void TwitterChatBot::stop() {
  pthread_cancel(PtweetData);

}
