
#include "ns3/nstime.h"
#include "ns3/attribute.h"
#include "ns3/log.h"
#include "ns3/system-thread.h"
#include <chrono>
#include <thread>
#include "ns3/simulator.h"
#include "ns3/tdma-rtc-trailer.h"
#include "ns3/system-wall-clock-ms.h"
#include "ns3/random-variable-stream.h"
#include "ns3/double.h"

namespace ns3
{
  namespace lorawan
  {
    NS_LOG_COMPONENT_DEFINE("TDMARTCTrailer");

    NS_OBJECT_ENSURE_REGISTERED(TDMARTCTrailer);

    TDMARTCTrailer::TDMARTCTrailer()
    {
    }
    TDMARTCTrailer::~TDMARTCTrailer()
    {
    }
    TypeId TDMARTCTrailer::GetTypeId (void)
    {
      static TypeId tid = TypeId ("ns3::TDMARTCTrailer")
        .SetParent<Trailer> ()
        .SetGroupName ("tdma")
        .AddConstructor<TDMARTCTrailer> ();
      return tid;
    }

    TypeId TDMARTCTrailer::GetInstanceTypeId (void) const
    {
      return GetTypeId ();
    }

    void TDMARTCTrailer::Print (std::ostream &os) const
    {
        os << "RTC: " << std::dec << m_rtc;
    }
    void TDMARTCTrailer::SetRTC (uint64_t rtc)
    {
        m_rtc=rtc;
    }
    uint64_t TDMARTCTrailer::GetRTC (void)
    {
        return m_rtc;
    }
    void TDMARTCTrailer::SetId (uint64_t dev_id)
    {
        m_dev_id=dev_id;
    }
    uint64_t TDMARTCTrailer::GetId (void)
    {
        return m_dev_id;
    }

    uint32_t TDMARTCTrailer::GetSerializedSize (void) const
    {
      return TDMA_LORAWAN_RTC_LENGTH;
    }

    void TDMARTCTrailer::Serialize (Buffer::Iterator start) const
    {
      NS_LOG_INFO("Serialize!");
      Buffer::Iterator i = start;

      i.Prev (TDMA_LORAWAN_RTC_LENGTH);
      i.WriteU64(m_rtc);
      i.WriteU64(m_dev_id);
      NS_LOG_INFO("Dev ID: " << m_dev_id);
    }

    uint32_t TDMARTCTrailer::Deserialize (Buffer::Iterator start)
    {
      NS_LOG_INFO("Deserialize!");
      Buffer::Iterator i = start;

      i.Prev (TDMA_LORAWAN_RTC_LENGTH);
      m_rtc = i.ReadU64();
      m_dev_id = i.ReadU64();
      return TDMA_LORAWAN_RTC_LENGTH;
    }
  } // namespace lorawan

} // namespace ns3
