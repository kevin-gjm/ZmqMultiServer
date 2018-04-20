#include "zmq_server.h"
#include <vector>
#include <exception>

using namespace  std;


#define UNKNOWN_REPLY 0
string g_sError;


void Server::Start(int thread_count)
{
    int iRet = UNKNOWN_REPLY;
    g_sError.clear();
    g_sError.append((char*)&iRet,sizeof(iRet));

    running_ = true;
    thread_.reserve(thread_count);
    for (int i = 0; i < thread_count; ++i)
    {
        thread_.push_back(std::thread(std::bind(&Server::ProxyRoutine,this)));
    }

    proxy_sock_->bind("inproc://proxy");

    zmq_pollitem_t pollitems [] =
    {
        { *sock_, 0, ZMQ_POLLIN, 0 },
        { *proxy_sock_, 0, ZMQ_POLLIN, 0 },
    };

    int rc;
    LOG(INFO)<<"Proxy start";
    while(running_)
    {
        rc = zmq::poll(pollitems, 2,-1);
        assert (rc >= 0);
        if (pollitems[0].revents & ZMQ_POLLIN)
        {
            zmq::multipart_t msg(*sock_);
            msg.send(*proxy_sock_);
        }
        if (pollitems[1].revents & ZMQ_POLLIN)
        {
            zmq::multipart_t msg(*proxy_sock_);
            msg.send(*sock_);
        }
    }

}


void Server::ProxyRoutine()
{

    std::shared_ptr<zmq::socket_t> receiver(new zmq::socket_t(*ctx_,ZMQ_DEALER));
    receiver->connect("inproc://proxy");
    LOG(INFO)<<"Work thread start";

    zmq_pollitem_t pollitems [] =
    {
        { *receiver, 0, ZMQ_POLLIN, 0 },
    };


    int rc;
    while(running_)
    {
        rc = zmq::poll(pollitems, 1,3000);
        assert (rc >= 0);
        if (pollitems[0].revents & ZMQ_POLLIN)
        {
            zmq::multipart_t oMsgRecv(*receiver);
            LOG(INFO)<<"Recv some info";
            if(oMsgRecv.size()<2)
            {
                continue;
            }

            zmq::message_t oIdentity = oMsgRecv.pop();
            ClientInfo client(oIdentity,true);

            std::shared_ptr<zmq::multipart_t> back_msg = client.MakeNewMsg();

            zmq::message_t oDataFrame = oMsgRecv.remove();
            std::string sResult;

            if(sResult.empty())
            {
                sResult = g_sError;
                LOG(ERROR)<<"Result is empty. Make it to error";
            }

            back_msg->addstr(sResult);
            bool bRet = back_msg->send(*receiver);
            if(bRet)
            {
                LOG(INFO)<<"Send to client success!";
            }else
            {
                LOG(ERROR)<<"Send to client error!";
            }
        }
    }
}


