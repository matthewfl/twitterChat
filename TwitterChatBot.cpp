#include "TwitterChatBot.h"
#include <pthread.h>
#include <curl/curl.h>
#include <boost/algorithm/string/replace.hpp> // used for minor clean up on tweet when saving

#include <matthewfl/json.hpp>

#include <string>
#include <stdlib.h>
#include <iostream>
#include <sstream>
using namespace std;

vector<vector<string> > SqlQuery(string, sqlite3*);

void TwitterChatBot::startFeed () {
  /// if feed is running do nothing
  if(!feedRunning) {
    feedRunning = true;
    pthread_create(&Pfeed, NULL, StreamThreadStart, (void*)this);
  }
}

void TwitterChatBot::stopFeed() {
  /// if feed not running do nothing
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
  cout << data << endl << endl;
  matthewfl::json j;
  stringstream message;
  j.prase(data);
  if(!j["delete"].empty()) {
    matthewfl::json d = j["delete"]["status"];
    message << "DELETE FROM tweet WHERE byID=" 
	    << d["user_id"].cast<matthewfl::json::Number>()
	    << " AND tweetID="
	    << d["id"].cast<matthewfl::json::Number>();
    
    // TODO: make a delete spool for deleting tweets out of the data base
    // TODO: add back in when saving of tweets is working
    // tweetQueue.push(message.str());
    return;
  }
  if(!j["text"].empty()) {
    
    matthewfl::json user = j["user"];
    message << "INSERT INTO tweet (text, byID, byName, tweetID, toID) VALUES (\""
	    << boost::replace_all_copy(j["text"].cast<matthewfl::json::String>(), "\"", "\\\"") // make this safe to insert
	    << "\", " << user["id"].cast<matthewfl::json::Number>() 
	    << ", \"" << user["screen_name"].cast<matthewfl::json::String>()
	    << "\", " << j["id"].cast<matthewfl::json::Number>() << ", ";
    if(j["in_reply_to_user_id"].empty())
      message << "NULL";
    else
      message << j["in_reply_to_user_id"].cast<matthewfl::json::Number>();
    message << ");";
    tweetQueue.push(message.str());
    cout << "@" << user["screen_name"].cast<matthewfl::json::String>() << " :  "
	 << j["text"].cast<matthewfl::json::String>() <<endl;
    if(!j["in_reply_to_user_id"].empty()) {
      cout << "\t\t\treply to: ";
      cout << j["in_reply_to_user_id"].cast<matthewfl::json::Number>()
	   << "  " << j["in_reply_to_screen_name"].cast<matthewfl::json::String>() << endl;
    }
  }
}

void * TwitterChatBot::AddTweetData (void * s) {
  TwitterChatBot * self = reinterpret_cast<TwitterChatBot*>(s);
  while(1) {
    while(!self->tweetQueue.empty()) {
      SqlQuery(self->tweetQueue.front(), self->Tweet);
      self->tweetQueue.pop();
    }
    cout << "database sleep\n";
    sleep(1);
  }
  return NULL;
}

unsigned int TwitterChatBot::queueSize() {
  /// sum all of the queues that are used
  return tweetQueue.size();
}

void TwitterChatBot::start() {
  if(!SystemRunning) {
    pthread_create(&PtweetData, NULL, AddTweetData, (void*)this);
  }
}

void TwitterChatBot::stop() {
  if(SystemRunning) {
    stopFeed();
    pthread_cancel(PtweetData);
  }
}


// database code


static int SqlCallBack(void * passed, int argc, char **argv, char **azColName) {
  vector<vector<string> > * p = reinterpret_cast<vector<vector<string> >* >(passed);
  vector<string> data;
  for(int i=0; i<argc;i++) {
    data.push_back(string(argv[i]));
  }
  p->push_back(data);
  return 0;
}

vector<vector<string> > SqlQuery (string q, sqlite3 * db) {
  char *error=0;
  vector<vector<string> > passed;
  if(SQLITE_OK != sqlite3_exec(db, q.c_str(), SqlCallBack, (void*)(&passed), &error)) {
    cerr<<"sql error: "<<error<<endl;
    sqlite3_free(error);
  }
  return passed;
}

