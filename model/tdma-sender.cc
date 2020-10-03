/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "tdma.h"
#include "ns3/tdma-sender.h"
#include "ns3/class-a-end-device-lorawan-mac.h"
#include "ns3/pointer.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/lora-net-device.h"
#include "ns3/simulator.h"
#include "string.h"
#include "ns3/lora-tag.h"
#include "ns3/simple-end-device-lora-phy.h"
#include "ns3/gateway-lora-phy.h"
using namespace std;

namespace ns3
{

	namespace lorawan
	{
		NS_LOG_COMPONENT_DEFINE("TDMASender");

		NS_OBJECT_ENSURE_REGISTERED(TDMASender);

		TypeId TDMASender::GetTypeId(void)
		{
			static TypeId tid =
				TypeId("ns3::TDMASender").SetParent<Application>().AddConstructor<TDMASender>().SetGroupName("tdma");
			return tid;
		}

		TDMASender::TDMASender()
		{
			//  NS_LOG_FUNCTION_NOARGS ();
			m_rate = 500;
			m_packet = 0;
			m_send_window = 0;
			time_t t_time(time(NULL));
			startTime = *localtime(&t_time);
			m_interval = 30 * 1;
			m_tracker = 0;
			deviceType = 0;
		}

		TDMASender::~TDMASender()
		{
			NS_LOG_FUNCTION_NOARGS();
		}
		void TDMASender::SetTDMAParams(TDMAParams params)
		{
			m_interval = params.interval;
			deviceType = params.deviceType;
		}

		void TDMASender::ScheduleReach(void)
		{
			//	NS_LOG_FUNCTION_NOARGS ();

			Ptr<RandomVariableStream> rv = CreateObjectWithAttributes<UniformRandomVariable>("Min", DoubleValue(30), "Max", DoubleValue(50));
			m_packet = rv->GetInteger();
			tm ltm = startTime;
			double sim_tm = Simulator::Now().GetSeconds();
			ltm.tm_sec += sim_tm;
			char now_str[40];
			fmt_date_tm(&ltm, now_str, 0);
			ltm.tm_sec -= sim_tm;

			Ptr<Packet> packet = Create<Packet>(m_packet);
			NS_LOG_INFO("Sent: " << this->m_packet << ", At: " << now_str << ", The packet: " << packet);
			if (deviceType == 1) //ED_A
			{
				m_mac->GetObject<ClassAEndDeviceLorawanMac>()->SetMType(LorawanMacHeader::CONFIRMED_DATA_UP);
			}
			m_mac->Send(packet);

			Simulator::Cancel(m_sendEvent);
			NS_LOG_INFO("Interval(seconds): " << m_interval);
			m_sendEvent = Simulator::Schedule(Seconds(m_interval), &TDMASender::SendPacket, this);
		}

		void TDMASender::SendPacket(void)
		{

			//  NS_LOG_FUNCTION_NOARGS ();
			tm ltm = startTime;
			if (m_tracker == 0)
			{
				Ptr<RandomVariableStream> rv = CreateObjectWithAttributes<UniformRandomVariable>("Min", DoubleValue(0), "Max", DoubleValue(5));
				m_tracker = rv->GetInteger();
			}

			double sim_tm = Simulator::Now().GetSeconds();
			ltm.tm_sec += sim_tm;
			char date_str[40];
			char now_str[40];
			fmt_date_tm(&ltm, now_str, 0);
			fmt_date_tm(&ltm, date_str, slot);
			ltm.tm_sec -= sim_tm;
			NS_LOG_INFO(
				"New window; TOA: " << toa << ", Slot: " << slot << ", Tracker: " << m_tracker << ", Now: " << now_str << ", Next: " << date_str);

			Simulator::Cancel(m_sendEvent);
			m_send_window = delta(1);
			m_sendEvent = Simulator::Schedule(Seconds(slot), &TDMASender::ScheduleReach, this);
		}

		void TDMASender::StartApplication(void)
		{
			tm ltm = startTime;
			NS_LOG_FUNCTION(this);
			slot = delta(m_node->GetId()); // in seconds
			toa = get_toa(m_node->GetId());

			char now_str[40];
			fmt_date_tm(&ltm, now_str, 0);
			NS_LOG_INFO("Starting Application with DELTA of " << slot << " seconds, At: " << now_str);

			Ptr<LoraNetDevice> loraNetDevice = m_node->GetDevice(0)->GetObject<LoraNetDevice>();

			// Make sure we have a MAC layer
			if (m_mac == 0)
			{
				// Assumes there's only one device
				m_mac = loraNetDevice->GetMac();
				NS_ASSERT(m_mac != 0);
			}

			// Schedule the next SendPacket event
			Simulator::Cancel(m_sendEvent);
			if (deviceType == 1) // ED_A
			{
				m_sendEvent = Simulator::Schedule(Seconds(2), &TDMASender::SendPacket, this);
			}else{
				Ptr<GatewayLoraPhy> phy = loraNetDevice->GetPhy()->GetObject<GatewayLoraPhy>();
				// phy->SetSpreadingFactor(12);
			}
		}

		void TDMASender::StopApplication(void)
		{
			NS_LOG_FUNCTION_NOARGS();
			NS_LOG_INFO("Stopping Application");
			Simulator::Cancel(m_sendEvent);
		}

		bool TDMASender::Receive(Ptr<NetDevice> loraNetDevice, Ptr<const Packet> packet, uint16_t protocol, const Address &sender)
		{
			Ptr<Packet> copy = packet->Copy();
			LoraTag s_tag;
			copy->RemovePacketTag(s_tag);
			double s_freq = s_tag.GetFrequency();
			uint8_t s_sf = s_tag.GetSpreadingFactor();
			NS_LOG_INFO("TxParams: " << s_freq << ", " << s_sf);

			const uint32_t size = packet->GetSize();
			NS_LOG_INFO("Packet size: " << size);

			uint8_t buffer[size] = {};
			packet->CopyData(buffer, size);
			buffer[size] = 0;
			NS_LOG_INFO("Data: " << buffer);

			long int ms = ts_now();

			NS_LOG_INFO("Time: " << ms);

			if (deviceType == 0) // Gateway -> send ACK
			{
				NS_LOG_INFO("Sending ACK");
				long int ms = ts_now();
				char ms_str[21];
				sprintf(ms_str, "%li", ms);
				string raw = "S";
				raw = raw.append(ms_str);
				// const unsigned char* pckt = (const unsigned char*)raw;
				uint32_t len = raw.length();
				NS_LOG_INFO("Len: " << len);

				Ptr<Packet> ackPacket = Create<Packet>((const uint8_t *)raw.c_str(), raw.length());
				LoraTag tag;
				ackPacket->RemovePacketTag(tag);
				tag.SetFrequency(s_freq);
				tag.SetSpreadingFactor(8);
				NS_LOG_INFO("DR1: " << s_tag.GetDataRate());
				NS_LOG_INFO("DR2: " << tag.GetDataRate());
				ackPacket->AddPacketTag(tag);
				// NS_LOG_INFO("Sent: "<<this->m_packet <<", At: " << now_str << ", The packet: " << packet);
				m_mac->Send(ackPacket);
			}

			return true;
		}
	} // namespace lorawan

} // namespace ns3
