#include "network_italo_italonet_ItalonetConfig.h"
#include <llarp.hpp>
#include "italonet_jni_common.hpp"

extern "C"
{
  JNIEXPORT jobject JNICALL
  Java_network_italo_italonet_ItalonetConfig_Obtain(JNIEnv* env, jclass)
  {
    auto conf = new llarp::Config();
    if (conf == nullptr)
      return nullptr;
    return env->NewDirectByteBuffer(conf, sizeof(llarp::Config));
  }

  JNIEXPORT void JNICALL
  Java_network_italo_italonet_ItalonetConfig_Free(JNIEnv* env, jclass, jobject buf)
  {
    auto ptr = FromBuffer<llarp::Config>(env, buf);
    delete ptr;
  }

  JNIEXPORT jboolean JNICALL
  Java_network_italo_italonet_ItalonetConfig_Load(JNIEnv* env, jobject self, jstring fname)
  {
    auto conf = GetImpl<llarp::Config>(env, self);
    if (conf == nullptr)
      return JNI_FALSE;
    return VisitStringAsStringView<jboolean>(env, fname, [conf](std::string_view val) -> jboolean {
      if (conf->Load(val, false, llarp::GetDefaultDataDir()))
        return JNI_TRUE;
      return JNI_FALSE;
    });
  }
}