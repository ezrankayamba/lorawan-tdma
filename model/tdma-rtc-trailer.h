/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef TDMA_RTC_TRAILER_H
#define TDMA_RTC_TRAILER_H

#include "ns3/application.h"
#include "ns3/nstime.h"
#include "ns3/attribute.h"
#include "ns3/object.h"
#include "ns3/system-wall-clock-ms.h"
#include "ns3/trailer.h"

static const uint16_t TDMA_LORAWAN_RTC_LENGTH = 20;

using namespace ns3;
// using namespace lorawan;

namespace ns3
{

  namespace lorawan
  {
    class TDMARTCTrailer : public Trailer
    {
    public:
      TDMARTCTrailer();
      ~TDMARTCTrailer();

      static TypeId GetTypeId (void);
      TypeId GetInstanceTypeId (void) const;
      void Print (std::ostream &os) const;
      uint32_t GetSerializedSize (void) const;
      void Serialize (Buffer::Iterator start) const;
      uint32_t Deserialize (Buffer::Iterator start);
      void SetRTC(uint64_t rtc);
      uint64_t GetRTC(void);
      void SetId(uint64_t dev_id);
      uint64_t GetId(void);
      private:
        int64_t m_rtc;
        int64_t m_dev_id;
    };
  } // namespace lorawan

} // namespace ns3

#endif /* TDMA_RTC_TRAILER_H */
