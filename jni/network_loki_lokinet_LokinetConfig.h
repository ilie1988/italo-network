/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class network_italo_italonet_ItalonetConfig */

#ifndef _Included_network_italo_italonet_ItalonetConfig
#define _Included_network_italo_italonet_ItalonetConfig
#ifdef __cplusplus
extern "C"
{
#endif
  /*
   * Class:     network_italo_italonet_ItalonetConfig
   * Method:    Obtain
   * Signature: ()Ljava/nio/Buffer;
   */
  JNIEXPORT jobject JNICALL
  Java_network_italo_italonet_ItalonetConfig_Obtain(JNIEnv*, jclass);

  /*
   * Class:     network_italo_italonet_ItalonetConfig
   * Method:    Free
   * Signature: (Ljava/nio/Buffer;)V
   */
  JNIEXPORT void JNICALL
  Java_network_italo_italonet_ItalonetConfig_Free(JNIEnv*, jclass, jobject);

  /*
   * Class:     network_italo_italonet_ItalonetConfig
   * Method:    Load
   * Signature: (Ljava/lang/String;)Z
   */
  JNIEXPORT jboolean JNICALL
  Java_network_italo_italonet_ItalonetConfig_Load(JNIEnv*, jobject, jstring);

#ifdef __cplusplus
}
#endif
#endif
