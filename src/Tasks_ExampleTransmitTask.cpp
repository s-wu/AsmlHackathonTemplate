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

    vector <string> vv[4];
    for (int i = 0; i < 32; i++)
    {
        string rn = "";
        for (int j = 0; j < 32; j++)
        {
            int d = i * (i - 31) + j * (j - 31);
            if (d >= -500 && d <= -220)
                rn += "1";
            else
                rn += "0";
        }
        vv[0].push_back(rn);
    }
    for (int i = 0; i < 32; i++)
    {
        string rn = "";
        for (int j = 0; j < 32; j++)
        {
            int d = i * (i - 31) + j * (j - 31);
            if (d >= -500 && d <= -450)
                rn += "1";
            else
                rn += "0";
        }
        vv[1].push_back(rn);
    }
    for (int i = 0; i < 32; i++)
    {
        string rn = "";
        for (int j = 0; j < 32; j++)
        {
            int d = i * (i - 31) + j * (j - 31);
            if (d >= -260 && d <= -220)
                rn += "1";
            else
                rn += "0";
        }
        vv[2].push_back(rn);
    }

    vv[3].push_back("0");
    

   int NWAIT = 12;
   if (++m_x >= NWAIT)
   {
        m_x = 0;       
   }
   int nt = m_x / 4;
   if (m_x % 4 == 3) nt = 3;
    string s;
    for (int i = 0; i < vv[nt].size(); i++)
    {
        for (int j = 0; j < vv[nt][i].length(); j++)
        {
            s += vv[nt][i][j];
        }
        s += '\n';
    }
    img.clear();
    for (int i = 0; i < vv[nt].size(); i++)
        img.push_back(vv[nt][i]);
   const char* c = s.c_str();
   String msg(c);
   MY_DEBUG_PRINTLN(msg);
   m_mesh.sendBroadcast( msg );
}

} // namespace Tasks
