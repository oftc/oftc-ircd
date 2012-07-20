The goal will be to allow server cycles in the network via multiple server
connections. The idea being that with multiple server connections users will
see fewer netsplits.

There are always two aspects, TheNetwork and TheServer.

Timing
------

Hybrid currently has a 10 second permitted delta in wallclock, this should be
lowered further to within 2 seconds (or less).

Hybrid also desperately tries to hold on to TCP connections allowing them to
gracefully recover sometimes over 5 mins. For the real time nature of IRC this
is unreasonable. Instead with multiple server connections tolerance for
non-responsive servers should match the permitted time delta or some other max
for RTT.

Perhaps ircd should propagate latency tables instead of mere alive packets so
leafs can pick best paths for delivering messages quickly.

Bursts
------

Hybrid has the notion of "burst"s which indicates the data transferred between
two servers upon connection. When server A connects to server B, A transfers
the state of the network as A sees it (all clients and channels that A knows
about) to B, B similarly transfers the state of the network as B sees it.

The simplest case is the cold start, where the started server simply knows no
clients or channels. In this case the leaf joining the network should request
all state (all clients, channels, and other services related data).

The more common and complicated case is when two partial networks are trying to
join together. This may be as simple as two servers each with existing clients
and channels, or two sets of multiple servers trying to merge. Inevitably
conflicts happen.

Conflict Resolution
-------------------

In Hybrid conflict resolution is resolved by the TS6 "protocol" (where TS
stands for TimeStamp). For instance Clients have a created TS, as does a
Channel and its TOPIC. A conflict is when two sides of the network try to
introduce two of the same entities. For instance a Client with the same nick.

In the event of a conflict the oldest of the two entities is preserved,
presuming the delta between them is larger than allowed time skew between
servers.

In the event of two Clients being introduced with the same nickname across
different sides of the network with relatively small TS delta neither client
is kept.

Channel ban-lists (misnomer since it includes invite and except lists)
generally take the union of both sides, reapplying each side of the networks
bans on the other.

These principles are relatively sound and would apply in a multi-server
connection environment, however with likely less conflicts since there are more
paths for the data to propagate.

Services Burst
--------------

Adding decentralized services to the software means that not only client and
channel state need to be propagated, but so will services state. However, since
modifications to services state require authentication there are fewer conflict
states that need to be handled. Account, Nick, and Channel could contain a
modification TS, where the newest one wins. Lists again function as the union
of both sides.

The question becomes how to handle Account/Nick/Channel drop/add between bursts.

Hypothetical situations, these all presume NetworkA and NetworkB rejoin.

Situations:


Actual Data Burst
-----------------

The data store is likely to be something like sqlite, I think that on burst
we just transfer the db entirely and let the conflict resolution work against
the two databases.

For this to work ideally it means that ircd state which almost certainly will
be stored in memory needs to also be duplicated to the db. I think a basic
transaction log will suffice, where memory is always right and after changes 
we perform Client.save() and Channel.save(). Each .save will coallesce with any
previous changes such that we can limit how often we write to the db. But
before we perform a burst we sync the db, and then sendfile.
