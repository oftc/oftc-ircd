/*
  Copyright (c) 2012 Stuart Walsh

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation
  files (the "Software"), to deal in the Software without
  restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following
  conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CHANNEL_H_INC
#define CHANNEL_H_INC

#include "stdinc.h"
#include <string>
#include <list>
#include <map>
#include "ircstring.h"
#include "baseclient.h"
#include "nuhmask.h"
#include "channelsection.h"

using std::list;
using std::string;
using std::map;

class Channel;

typedef shared_ptr<Channel> ChannelPtr;

enum MembershipFlags
{
  Member = 0x1,
  Voice = 0x2,
  ChanOp = 0x4
};

struct Membership
{
  ClientPtr client;
  ChannelPtr channel;
  MembershipFlags flags;
};

typedef unordered_map<irc_string, ChannelPtr, irc_hash> ChannelNameHash;
typedef map<Channel *, ChannelPtr> ChannelList;
typedef map<ClientPtr, Membership> ChannelMemberList;

class Channel
{
private:
  static ChannelSection config;
  static ChannelNameHash names;
  static ChannelList channels;

  ChannelMemberList members;
  NuhMaskList bans;
  NuhMaskList invexes;
  NuhMaskList quiets;
  NuhMaskList excepts;

  irc_string name;
  string topic;

  bool moderated;
  bool invite_only;
  bool no_external_msgs;
  bool private_chan;
  bool secret;
  bool topic_op_only;
  bool secure;
public:
  // Events
  static Event<ChannelPtr, ClientPtr> joining;
  static Event<ChannelPtr, ClientPtr> joined;

  // Ctor/dtor
  Channel();
  ~Channel();

  // statics
  static void init();
  inline static string supported_modes() { return "imnpstS"; }
  inline static unsigned int get_topiclen() { return config.get_topiclen(); }

  // list managers
  static void add(const ChannelPtr);
  static ChannelPtr find(const irc_string);
  static void del(const ChannelPtr);

  // tests
  inline bool is_moderated() const { return moderated; }
  inline bool is_invite_only() const { return invite_only; }
  inline bool is_no_external_msgs() const { return no_external_msgs; }
  inline bool is_private() const { return private_chan; }
  inline bool is_secret() const { return secret; }
  inline bool is_topic_op_only() const { return topic_op_only; }
  inline bool is_secure() const { return secure; }

  // getters
  NuhMaskList& get_bans();
  NuhMaskList& get_quiets();
  NuhMaskList& get_excepts();
  NuhMaskList& get_invexes();
  irc_string get_name() const;
  ChannelMemberList get_members() const;
  string get_topic() const;

  // setters
  void set_name(const irc_string);
  void set_mode_char(char, bool);
  void set_topic(const string);
  
  // members
  void add_member(const ClientPtr);
  bool is_member(const ClientPtr);
  void remove_member(const ClientPtr);
  void send_names(const ClientPtr);
  void send(const ClientPtr, const string);
  void send_common(const ClientPtr, const string);
  irc_string str() const;
 };

#endif
