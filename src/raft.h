/******************************
* Raft
* Jacob English
* je787413@ohio.edu
*******************************/
#include "contiki.h"

#define UDP_PORT 1234 //UDP Broadcast Port for messaging
#define MIN_TIMEOUT 5 //minimum value for timeout
#define MAX_TIMEOUT 10 //maximum value for timeout
#define LEADER_SEND_INTERVAL (MIN_TIMEOUT / 2)
#define TOTAL_NODES 2 //total number of nodes in network

enum states {follower, candidate, leader};
enum msg_types {heartbeat, election, vote};

struct Raft {
  uint32_t term;
  uint8_t macAddr[8];
  uint8_t votedFor[8];
  uint8_t timeout;
  enum states state;
  uint8_t totalVotes;
};

// default message struct to determine type and term on incoming messages
struct Msg {
  enum msg_types type;
  uint32_t term;
  uint8_t from[8];
};

// heartbeat message
struct Heartbeat {
  enum msg_types type;
  uint32_t term;
  uint8_t from[8];
  uint8_t prevLogIndex; // Not sure what to do with these quite yet
  uint8_t prevLogTerm;  //
  uint8_t entries;      //
  uint8_t leaderCommit; //
};
void build_heartbeat(struct Heartbeat *heart, uint32_t term, uint8_t from[], uint8_t prevLogIndex,
               uint8_t prevLogTerm, uint8_t entries, uint8_t leaderCommit);

// start election message
struct Election {
  enum msg_types type;
  uint32_t term;
  uint8_t from[8];
  uint8_t lastLogIndex; // Same as above
  uint8_t lastLogTerm;  //
};
void build_election(struct Election *elect, uint32_t term, uint8_t from[], uint8_t lastLogIndex, uint8_t lastLogTerm);

// vote response for election
struct Vote {
  enum msg_types type;
  uint32_t term;
  uint8_t from[8];
  uint8_t voteFor[8];
  bool voteGranted;
};
void build_vote(struct Vote *vote, uint32_t term, uint8_t from[], uint8_t voteFor[], bool voteGranted);

void raft_init(struct Raft *node);
void raft_print(struct Raft *node);
void raft_set_follower(struct Raft *node);
void raft_set_candidate(struct Raft *node);
void raft_set_leader(struct Raft *node);

bool mac_compare(uint8_t a[], uint8_t b[]);

void msg_print(uint32_t currTerm, const uip_ipaddr_t *from, struct Msg *msg);
void heartbeat_print(struct Heartbeat *heart);
void election_print(struct Election *elect);
void vote_print(struct Vote *vote);

void call_election(struct Raft *node);
void send_vote(struct Raft *node);
void send_heartbeat(struct Raft *node);
