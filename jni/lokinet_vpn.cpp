#include "network_italo_italonet_ItalonetVPN.h"
#include "italonet_jni_vpnio.hpp"
#include "italonet_jni_common.hpp"
#include <net/ip.hpp>
#include <net/ip_packet.hpp>
#include <string_view>

extern "C"
{
  JNIEXPORT jint JNICALL
  Java_network_italo_italonet_ItalonetVPN_PacketSize(JNIEnv*, jclass)
  {
    return llarp::net::IPPacket::MaxSize;
  }

  JNIEXPORT jobject JNICALL
  Java_network_italo_italonet_ItalonetVPN_Alloc(JNIEnv* env, jclass)
  {
    italonet_jni_vpnio* vpn = new italonet_jni_vpnio();
    return env->NewDirectByteBuffer(vpn, sizeof(italonet_jni_vpnio));
  }

  JNIEXPORT void JNICALL
  Java_network_italo_italonet_ItalonetVPN_Free(JNIEnv* env, jclass, jobject buf)
  {
    italonet_jni_vpnio* vpn = FromBuffer<italonet_jni_vpnio>(env, buf);
    if (vpn == nullptr)
      return;
    delete vpn;
  }
  JNIEXPORT void JNICALL
  Java_network_italo_italonet_ItalonetVPN_Stop(JNIEnv* env, jobject self)
  {
    italonet_jni_vpnio* vpn = GetImpl<italonet_jni_vpnio>(env, self);
    if (vpn)
    {
      vpn->Close();
    }
  }

  JNIEXPORT jint JNICALL
  Java_network_italo_italonet_ItalonetVPN_ReadPkt(JNIEnv* env, jobject self, jobject pkt)
  {
    italonet_jni_vpnio* vpn = GetImpl<italonet_jni_vpnio>(env, self);
    if (vpn == nullptr)
      return -1;
    void* pktbuf = env->GetDirectBufferAddress(pkt);
    auto pktlen = env->GetDirectBufferCapacity(pkt);
    if (pktbuf == nullptr)
      return -1;
    return vpn->ReadPacket(pktbuf, pktlen);
  }

  JNIEXPORT jboolean JNICALL
  Java_network_italo_italonet_ItalonetVPN_WritePkt(JNIEnv* env, jobject self, jobject pkt)
  {
    italonet_jni_vpnio* vpn = GetImpl<italonet_jni_vpnio>(env, self);
    if (vpn == nullptr)
      return false;
    void* pktbuf = env->GetDirectBufferAddress(pkt);
    auto pktlen = env->GetDirectBufferCapacity(pkt);
    if (pktbuf == nullptr)
      return false;
    return vpn->WritePacket(pktbuf, pktlen);
  }

  JNIEXPORT void JNICALL
  Java_network_italo_italonet_ItalonetVPN_SetInfo(JNIEnv* env, jobject self, jobject info)
  {
    italonet_jni_vpnio* vpn = GetImpl<italonet_jni_vpnio>(env, self);
    if (vpn == nullptr)
      return;
    VisitObjectMemberStringAsStringView<bool>(
        env, info, "ifaddr", [vpn](std::string_view val) -> bool {
          vpn->SetIfAddr(val);
          return true;
        });
    VisitObjectMemberStringAsStringView<bool>(
        env, info, "ifname", [vpn](std::string_view val) -> bool {
          vpn->SetIfName(val);
          return true;
        });
    vpn->info.netmask = GetObjectMemberAsInt<uint8_t>(env, info, "netmask");
  }
}