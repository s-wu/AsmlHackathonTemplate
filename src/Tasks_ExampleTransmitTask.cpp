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

ExampleTransmitTask::ExampleTransmitTask(Facilities::MeshNetwork& mesh, vector<string>& v) :
   Task(TASK_SECOND * 1 , TASK_FOREVER, std::bind(&ExampleTransmitTask::execute, this)),
   m_mesh(mesh),
   img(v),
   m_x(0)
{
}

void ExampleTransmitTask::execute()
{
    if (m_mesh.getNodeIndex().first != 0)
        return;

   int NWAIT = 3;
   if (++m_x >= NWAIT)
   {
       m_x -= NWAIT;
       bool found = false;
       for (int i = 0; i < img.size(); i++)
       {
           if (found) break;
           for (int j = 0; j < img[i].length(); j++)
           {
               if (img[i][j] == '1')
                img[i][j] = '0';
                else
                {
                    img[i][j] = '1';
                    found = true;
                    break;
                }
           }
       }
   }
    string s;
    for (int i = 0; i < img.size(); i++)
    {
        for (int j = 0; j < img[i].length(); j++)
        {
            s += img[i][j];
        }
        s += '\n';
    }
   const char* c = s.c_str();
   String msg(c);
   MY_DEBUG_PRINTLN(msg);
   m_mesh.sendBroadcast( msg );
}

} // namespace Tasks
