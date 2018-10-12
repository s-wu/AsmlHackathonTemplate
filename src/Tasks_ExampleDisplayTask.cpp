//
//! \file
//
// Created by Sander van Woensel / Umut Uyumaz
// Copyright (c) 2018 ASML Netherlands B.V. All rights reserved.
//
//! Example Task to output something to the LED Matrix.
#include "Tasks_ExampleDisplayTask.hpp"

#include "Debug.hpp"
#include "Facilities_MeshNetwork.hpp"

#include <LEDMatrixDriver.hpp>

#include <functional>

namespace Tasks {


const int ExampleDisplayTask::LEDMATRIX_WIDTH = 31;
const int ExampleDisplayTask::LEDMATRIX_HEIGHT = 7;
const int ExampleDisplayTask::LEDMATRIX_SEGMENTS = 4;
const int ExampleDisplayTask::LEDMATRIX_INTENSITY = 5;
const int ExampleDisplayTask::LEDMATRIX_CS_PIN = 16;
const unsigned long ExampleDisplayTask::POLL_DELAY_MS = 100;

//! Initializes the LED Matrix display.
ExampleDisplayTask::ExampleDisplayTask(Facilities::MeshNetwork& mesh) :
   Task(POLL_DELAY_MS , TASK_FOREVER, std::bind(&ExampleDisplayTask::execute, this)),
   m_mesh(mesh),
   m_lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN),
   m_x(0)
{
   m_lmd.setEnabled(true);
   m_lmd.setIntensity(LEDMATRIX_INTENSITY);

   m_mesh.onReceive(std::bind(&ExampleDisplayTask::receivedCb, this, std::placeholders::_1, std::placeholders::_2));
}

void ExampleDisplayTask::display (int x, int y)
{
    m_lmd.setPixel(x ^ 7, y, true);
}

//! Update display
void ExampleDisplayTask::execute()
{
   //MY_DEBUG_PRINTLN("Executing");
   m_lmd.clear();
   for (int i = 0; i < 32; i++)
   {
        for (int j = 0; j < 32; j++)
            if (i & (1 << (j & 7)))
                display(i, j);
   }
   for (int y = 0; y < 8; y++)
    display (m_x, y);
   m_lmd.display();
}

void ExampleDisplayTask::receivedCb(Facilities::MeshNetwork::NodeId nodeId, String& msg)
{
   //MY_DEBUG_PRINTLN("Received data in ExampleDisplayTask");
   MY_DEBUG_PRINTLN(msg);

   if(++m_x>LEDMATRIX_WIDTH)
   {
      m_x=0;
   }
}

} // namespace Tasks
