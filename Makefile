CXXFLAGS = -O2 -Wall -fmessage-length=0

OBJS = twitterChat.o TwitterChatBot.o

LIBS = -lhoard -lpthread -lcurl -lsqlite3
# download libs from:
#	hoard.org # not needed but helps
#	boost.org
#	sqlite.org
#	http://blog.beef.de/projects/tinyjson/
# installed curl & libcurl

TARGET = twitterChat

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)
	LDFLAGS='curl-config --libs'

clean:
	rm -f $(OBJS) $(TARGET)

