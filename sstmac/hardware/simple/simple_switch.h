#ifndef SIMPLE_SWITCH_H
#define SIMPLE_SWITCH_H

#include <sstmac/common/event_handler.h>
#include <sstmac/common/event_scheduler.h>
#include <sstmac/common/messages/sst_message_fwd.h>
#include <sstmac/hardware/nic/nic_fwd.h>
#include <sstmac/hardware/switch/network_switch.h>
#include <sprockit/unordered.h>

namespace sstmac {
namespace hw {

class simple_switch :
  public network_switch
{

 public:
  simple_switch(sprockit::sim_parameters* params, uint64_t id, event_manager* mgr);

  int
  queue_length(int port) const {
    return 0;
  }

  virtual void
  connect_output(int src_outport,
    int dst_inport,
    connectable *mod,
    config *cfg);

  virtual void
  connect_input(int src_outport,
    int dst_inport,
    connectable *mod,
    config *cfg);

  std::vector<switch_id>
  connected_switches() const;

  /**
   Cast message and pass to #send
   @param msg Incoming message (should cast to packet_train)
   */
  virtual void
  handle(event* ev);

  virtual std::string
  to_string() const {
    return "simple switch";
  }

  virtual
  ~simple_switch();

  timestamp
  hop_latency() const {
    return hop_latency_;
  }

  double
  inverse_bw() const {
    return inverse_bw_;
  }

  timestamp
  lookahead() const;

  double
  hop_bandwidth() const {
    return 1.0/inverse_bw_;
  }

 protected:
  virtual void
  connect_injector(int src_outport, int dst_inport, event_handler* nic);

  virtual void
  connect_ejector(int src_outport, int dst_inport, event_handler* nic);

  void
  add_switch(connectable* sw);

 protected:
  node_id my_start_;
  node_id my_end_;

  double inj_bw_inverse_;

  timestamp inj_lat_;

  double inverse_bw_;

  double inv_min_bw_;

  timestamp hop_latency_;

  topology* top_;

  spkt_unordered_map<node_id, network_switch*> neighbors_;

  spkt_unordered_map<node_id, nic*> nics_;


 private:
  void send_to_nic(timestamp delay, node_id dst, message* msg);
  void send_to_switch(timestamp delay, node_id dst, message* msg);

};

}
}

#endif // SIMPLE_SWITCH_H