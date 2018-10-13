//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Mesh Network wrapper class to provide a container to add specific
//! mesh network behaviour.

#include "Facilities_MeshNetwork.hpp"


#include "Debug.hpp"
#include "painlessMesh.h"


namespace Facilities {

const uint16_t MeshNetwork::PORT = 5555;

//! Construct only.
//! \note Does not construct and initialize in one go to be able to initialize after serial debug port has been opened.
MeshNetwork::MeshNetwork()
{
   m_mesh.onReceive(std::bind(&MeshNetwork::receivedCb, this, std::placeholders::_1, std::placeholders::_2));
   m_mesh.onNewConnection(std::bind(&MeshNetwork::newConnect, this));
   //m_mesh.onChangedConnections(...);
   //m_mesh.onNodeTimeAdjusted(...);
}

// Initialize mesh network.
void MeshNetwork::initialize(const __FlashStringHelper *prefix, const __FlashStringHelper *password, Scheduler& taskScheduler)
{
   // Set debug messages before init() so that you can see startup messages.
   m_mesh.setDebugMsgTypes( ERROR | STARTUP );  // To enable all: ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE
   m_mesh.init( prefix, password, &taskScheduler, MeshNetwork::PORT );
}

//! Update mesh; forward call to painlessMesh.
void MeshNetwork::update()
{
   m_mesh.update();
}

void MeshNetwork::sendBroadcast(String &message)
{
   MY_DEBUG_PRINT("Broadcasting message: "); MY_DEBUG_PRINTLN(message);
   m_mesh.sendBroadcast(message, false); // false: Do not include self.
}

MeshNetwork::NodeId MeshNetwork::getMyNodeId()
{
   return m_mesh.getNodeId();
}

void MeshNetwork::pumpNodeIndex()
{
    while (indt.size() >= 10)
    {
        indt.erase(indt.begin());
    }
    auto id = m_mesh.getNodeId();
    vector <uint32_t> v;
    for (auto x : m_mesh.getNodeList())
    {
        if (x == FILLIN) continue;
        v.push_back(x);
        //MY_DEBUG_PRINTLN(x);
    }
    sort(v.begin(), v.end());
    int nc = 0;
    for (auto x : v)
        if (x < id)
            nc++;
    
    //MY_DEBUG_PRINTLN("INDEX");
    //MY_DEBUG_PRINTLN(nc);
    int ntot = 1 + v.size();
    indt.push_back(make_pair (nc, ntot));
}

pair <int, int> MeshNetwork::getNodeIndex()
{
    //MY_DEBUG_PRINTLN(ntot);
    int chi = 0, rhi = 1;
    for (int i = 0; i < indt.size(); i++)
    {
        chi = max (chi, indt[i].first);
        rhi = max (rhi, indt[i].second);
    }
    return make_pair (chi, rhi);
}

int MeshNetwork::millis()
{
    return m_mesh.getNodeTime() / 1000;
}


void MeshNetwork::onReceive(receivedCallback_t receivedCallback)
{
   m_mesh.onReceive(receivedCallback);
}

void MeshNetwork::receivedCb(NodeId transmitterNodeId, String& msg)
{
   MY_DEBUG_PRINTF("Data received from node: %u; msg: %s\n", transmitterNodeId, msg.c_str());
}

void MeshNetwork::newConnect()
{
   MY_DEBUG_PRINTF("New connection!!");
}


} // namespace Facilities
