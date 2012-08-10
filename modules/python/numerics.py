#  Copyright (c) 2012 Stuart Walsh
#
#  Permission is hereby granted, free of charge, to any person
#  obtaining a copy of this software and associated documentation
#  files (the "Software"), to deal in the Software without
#  restriction, including without limitation the rights to use,
#  copy, modify, merge, publish, distribute, sublicense, and/or sell
#  copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following
#  conditions:
#
#  The above copyright notice and this permission notice shall be
#  included in all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
#  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
#  OTHER DEALINGS IN THE SOFTWARE.

RPL_WELCOME = 001
RPL_YOURHOST = 002
RPL_CREATED = 003
RPL_MYINFO = 004
RPL_WHOISUSER = 311
RPL_WHOISSERVER = 312
RPL_WHOISIDLE = 317
RPL_ENDOFWHOIS = 318
RPL_NAMESREPLY = 353
RPL_ENDOFNAMES = 366
RPL_ENDOFWHOWAS = 369
RPL_MOTD = 372
RPL_MOTDSTART = 375
RPL_MOTDEND = 376

#error replies
ERR_NOSUCHNICK = 401
ERR_NOSUCHCHANNEL = 403
ERR_WASNOSUCHNICK = 406
ERR_UNKNOWNCOMMAND = 421
ERR_NOMOTD = 422
ERR_NICKNAMEINUSE = 433
ERR_NEEDMOREPARAMS = 461
ERR_ALREADYREGISTERED = 462
ERR_BADCHANNAME = 479
ERR_UMODEUNKNOWNFLAG = 501
ERR_USERSDONTMATCH = 502
