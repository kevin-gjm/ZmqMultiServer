#include <string>
#include <iostream>
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <glog/logging.h>
#include "zmq_server.h"
extern "C" {
#include "ini.h"
}


using namespace std;

zmq::context_t *g_pContext;



int main()
{
    google::InitGoogleLogging("FeatureDetectServer");
    google::InstallFailureSignalHandler();
    google::SetLogDestination(google::GLOG_INFO, "log");
    google::SetLogSymlink(google::GLOG_INFO,"");
    FLAGS_alsologtostderr = true;
    FLAGS_colorlogtostderr=true;

#if _WIN32
    // ini_t *config = ini_load("D:\\SmartPark_PublicSetting.ini");
    ini_t *config = ini_load("./config.ini");
#else
    ini_t *config = ini_load("/etc/smartpark/SmartPark_PublicSetting.ini");
#endif
    if(config == NULL)
        return -1;
    int port;
    if (!ini_sget(config, "Public_Server_IP_PORT", "feature_compare_server_port", "%d", &port))return -1;

    ini_free(config);

    zmq::context_t ctx;
    g_pContext = &ctx;

    Server oServer(&ctx,"",port);

    oServer.Start(5);

    return 0;

}
