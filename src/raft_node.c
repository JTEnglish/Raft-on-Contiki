/******************************
* Contiki Raft Node
* Jacob English
* je787413@ohio.edu
*******************************/

#include "contiki.h"
#include "sys/etimer.h"
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"

#include "dev/leds.h"
#include "simple-udp.h"
#include "sys/timer.h"

#include "raft.h"

#include <stdio.h>
#include <stdint.h>

static struct Raft node;
bool init = false;

static struct simple_udp_connection broadcast_connection;
int recv = 0; //count on received messages
bool buzzer_flag = false;

/*---------------------------------------------------------------------------*/
PROCESS(raft_node_process, "UDP broadcast raft node process");
AUTOSTART_PROCESSES(&raft_node_process);
/*---------------------------------------------------------------------------*/
static void
receiver(struct simple_udp_connection *c,
         const uip_ipaddr_t *sender_addr,
         uint16_t sender_port,
         const uip_ipaddr_t *receiver_addr,
         uint16_t receiver_port,
         const uint8_t *data,
         uint16_t datalen) {

  struct Msg *msg = (struct Msg *)data;

  if (msg->term > node.term) { //got msg with higer term, update term and change to follower state
    node.term = msg->term;
    node.state = follower;
    // node.votedFor = 0; //reset voted for
  }
  else if (msg->term < node.term) { //ignore states less than node state
    return;
  }

  switch (node.state) {
    case follower:
      //election
      if (msg->type == election) {
        struct Election *elect = (struct Election *)data;
        //reset timer
        //if vote not used (does it have to be the address? -> bool?)

      }
      //heartbeat
      if (msg->type == heartbeat) {
        struct Heartbeat *heart = (struct Heartbeat *)data;
        //reset timer
      }
      //log stuff later on
      break;
    case candidate:
      //vote response
      if (msg->type == vote) {
        struct Vote *vote = (struct Vote *)data;
        //vote is for this node
        if ((uip_ipaddr_cmp(&vote->voteFor, receiver_addr)) && (vote->voteGranted)) {
          //increment vote count
          //if vote count is majority, change to leader & send heartbeat
        }
      }
      break;
    case leader:
      //log stuff later on
      break;
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(raft_node_process, ev, data) {
  static struct etimer timer;
  uip_ipaddr_t addr;

  PROCESS_BEGIN();

  printf("--BROADCAST RAFT NODE PROCESS BEGIN--\n");

  if (!init) {
    raft_init(&node);
    init = true;
  }

  simple_udp_register(&broadcast_connection, UDP_PORT,
                      NULL, UDP_PORT,
                      receiver);

  while(1) {
    etimer_set(&timer, 2 * CLOCK_SECOND);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));

    node.term += 1;

    printf("DELAY: %d\n", (2 * CLOCK_SECOND));
    printf("NODE: {term: %ld, timeout: %d, state: %d}\n", node.term, node.timeout, node.state);


    switch (node.state) {
      case follower:
        break;
      case candidate:
        break;
      case leader:
        break;
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
