//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Example Task
#include "Tasks_ExampleTransmitTask.hpp"

#include "Debug.hpp"
#include "Facilities_MeshNetwork.hpp"

#include <functional>

namespace Tasks {

ExampleTransmitTask::ExampleTransmitTask(Facilities::MeshNetwork& mesh, vector<String>& v) :
   Task(TASK_SECOND * 1 , TASK_FOREVER, std::bind(&ExampleTransmitTask::execute, this)),
   m_mesh(mesh),
   img(v),
   m_x(0)
{
}

void ExampleTransmitTask::execute()
{
    MY_DEBUG_PRINT("Currently at ");
    MY_DEBUG_PRINTLN(m_mesh.millis());
    MY_DEBUG_PRINT(img.size());
    MY_DEBUG_PRINTLN(" jobs in queue");
    if (m_mesh.getNodeIndex().first != 0)
        return;
    MY_DEBUG_PRINTLN("ROOT");

   /*int NWAIT = 1;
   while (img.size() < 5)
   {
       String sp = img.back();
       for (int i = 0; i < sp.length(); i++)
       {
           if (sp[i] == '0')
           {
            sp[i] = '1';
            break;
           }
           if (sp[i] == '1')
            sp[i] = '0';
       }
       m_x -= NWAIT;
       img.push_back(sp);
   }*/

    string s = "";
    for (int i = 0; i < img.size(); i++)
    {
        s += img[i].c_str();
        s += ',';
    }
    const char* c = s.c_str();
    String m(c);
    m_mesh.sendBroadcast(m);

    /*s = "";
    auto f = m_mesh.millis();
    while (f)
    {
        s = (char ('0' + f % 10)) + s;
    }
    s = "t" + s;
    c = s.c_str();
    String m2(c);
    m_mesh.sendBroadcast(m2);*/
}

} // namespace Tasks
