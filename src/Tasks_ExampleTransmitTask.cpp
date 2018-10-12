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


ExampleTransmitTask::ExampleTransmitTask(Facilities::MeshNetwork& mesh) :
   Task(TASK_SECOND * 1 , TASK_FOREVER, std::bind(&ExampleTransmitTask::execute, this)),
   m_mesh(mesh)
{

}

void ExampleTransmitTask::execute()
{
    if (m_mesh.getNodeIndex().first != 0)
        return;
    string s;
    int N = 3, M = 3;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if ((i + j) % 2 == 0 && i + j < 4)
                s += '1';
            else
                s += '0';
        }
        s += '\n';
    }
   const char* c = s.c_str();
   String msg(c);
   MY_DEBUG_PRINTLN(msg);
   m_mesh.sendBroadcast( msg );
}

} // namespace Tasks
