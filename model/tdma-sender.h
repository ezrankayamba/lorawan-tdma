/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef TDMA_SENDER_H
#define TDMA_SENDER_H

#include "ns3/application.h"
#include "ns3/nstime.h"
#include "ns3/lorawan-mac.h"
#include "ns3/attribute.h"
#include "ns3/tdma-lorawan-rtc.h"
#include "ns3/system-wall-clock-ms.h"

using namespace ns3;
using namespace lorawan;

namespace ns3
{

  namespace lorawan
  {
    struct TDMAParams
    {
      uint16_t interval;
      uint8_t deviceType;
      uint16_t mSyncByDevId;
    };
    class TDMASender : public Application
    {
    public:
      TDMASender();
      //	TDMASender (Time sendTime);
      ~TDMASender();

      static TypeId GetTypeId(void);

      void ScheduleReach(void);
      bool Receive(Ptr<NetDevice> loraNetDevice, Ptr<const Packet> packet, uint16_t protocol, const Address &sender);
      void TxFinished(Ptr<const Packet> packet);
      
      /**
   * Send a packet using the LoraNetDevice's Send method.
   */
      void SendPacket(void);
      void SetTDMAParams(TDMAParams params);

      /**
   * Start the application by scheduling the first SendPacket event.
   */
      void StartApplication(void);

      /**
   * Stop the application.
   */
      void StopApplication(void);

    private:
      uint16_t m_rate;
      uint16_t m_packet;
      double m_send_window;
      uint16_t m_tracker;
      uint16_t node_id;
      double slot;
      double toa;
      tm startTime;
      uint16_t m_interval;
      uint8_t deviceType;
      uint16_t m_syncByDevId;


      EventId m_sendEvent;
      Ptr<LorawanMac> m_mac;
      SystemWallClockMs m_clock;
      uint64_t m_Tx;
    };

  } // namespace lorawan

} // namespace ns3

#endif /* TDMA_H */
