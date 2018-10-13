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
#include <chrono>
#include <time.h>

namespace Tasks {


const int ExampleDisplayTask::LEDMATRIX_WIDTH = 31;
const int ExampleDisplayTask::LEDMATRIX_HEIGHT = 7;
const int ExampleDisplayTask::LEDMATRIX_SEGMENTS = 4;
const int ExampleDisplayTask::LEDMATRIX_INTENSITY = 5;
const int ExampleDisplayTask::LEDMATRIX_CS_PIN = 16;
const unsigned long ExampleDisplayTask::POLL_DELAY_MS = 100;
const uint64_t ExampleDisplayTask::IMAGES[] = {
  0x6666667e66663c00,
  0x3e66663e66663e00,
  0x3c66060606663c00,
  0x3e66666666663e00,
  0x7e06063e06067e00,
  0x0606063e06067e00,
  0x3c66760606663c00,
  0x6666667e66666600,
  0x3c18181818183c00,
  0x1c36363030307800,
  0x66361e0e1e366600,
  0x7e06060606060600,
  0xc6c6c6d6feeec600,
  0xc6c6e6f6decec600,
  0x3c66666666663c00,
  0x06063e6666663e00,
  0x603c766666663c00,
  0x66361e3e66663e00,
  0x3c66603c06663c00,
  0x18181818185a7e00,
  0x7c66666666666600,
  0x183c666666666600,
  0xc6eefed6c6c6c600,
  0xc6c66c386cc6c600,
  0x1818183c66666600,
  0x7e060c1830607e00
};

//! Initializes the LED Matrix display.
ExampleDisplayTask::ExampleDisplayTask(Facilities::MeshNetwork& mesh, vector<String>& v) :
   Task(POLL_DELAY_MS , TASK_FOREVER, std::bind(&ExampleDisplayTask::execute, this)),
   m_mesh(mesh),
   m_lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN),
   m_x(-1e9),
   img(v),
   istart(0)
{
   m_lmd.setEnabled(true);
   m_lmd.setIntensity(LEDMATRIX_INTENSITY);

   m_mesh.onReceive(std::bind(&ExampleDisplayTask::receivedCb, this, std::placeholders::_1, std::placeholders::_2));
    img.push_back("100\n000\n000\n0\n");
}

int gv (string s)
{
   int r = 0;
   for (int i = 0; i < s.length(); i++)
      r = 10 * r + (s[i] - '0');
    return r;
}

vector <string> process (String& msg)
{
   vector <string> res;
   string s = "";
   for (char c : msg)
   {
       if (c == '\n')
       {
           res.push_back(s);
           s = "";
       }
       else
            s += c;
   }
   return res;
}

void ExampleDisplayTask::display (int x, int y)
{
    m_lmd.setPixel(x ^ 7, y, true);
}

int ExampleDisplayTask::scale(vector <string> v)
{
   if (!v.size() || !v[0].length())
      return 1;
   int N = v.size(), M = v[0].length();
   int nc = 32, mc = 8 * m_mesh.getNodeIndex().second;
   return min (nc / N, mc / M);
}

int ExampleDisplayTask::taskhi()
{
    if (img.size() <= 1)
    {
        return m_mesh.millis() + 2000;
    }
    vector <string> v = process(img.back());
    return gv (v.back());
}

void ExampleDisplayTask::addTask()
{
    if (m_mesh.getNodeIndex().first != 0)
      return;
    img.push_back(img.back());
    for (int i = 0; i < img.back().length(); i++)
    {
      if (img.back()[i] == '0')
      {
        img.back()[i] = '1';
        break;
      }
      else if (img.back()[i] == '1')
        img.back()[i] = '0';
    }
    vector <string> v = process(img.back());
    v.pop_back();
    int ct = taskhi();
    ct += 1000;
    string s = "";
    while (ct)
    {
      s = (char) (ct % 10 + '0') + s;
      ct /= 10;
    }
    v.push_back(s);

    String ans = "";
    for (int i = 0; i < v.size(); i++)
    {
      const char* c = v[i].c_str();
      String m(c);
      m += "\n";
      ans += m;
    }
    img.push_back(ans);
}

void ExampleDisplayTask::addTask(vector <string> v)
{
    int ct = taskhi() + 1000;
    string s = "";
    while (ct)
    {
      s = (char) (ct % 10 + '0') + s;
      ct /= 10;
    }
    v.push_back(s);

    String ans = "";
    for (int i = 0; i < v.size(); i++)
    {
      const char* c = v[i].c_str();
      String m(c);
      m += "\n";
      ans += m;
    }
    img.push_back(ans);
}

void ExampleDisplayTask::pushWord(string s)
{
    vector <string> rs;
    rs.push_back("0");
    addTask(rs);
    for (int i = 0; i < s.length(); i++)
    {
        char c = s[i];
        uint64_t ival = IMAGES[(c - 'a')];
        vector <string> v;
        for (int j = 0; j < 8; j++)
        {
            v.push_back("");
            for (int k = 0; k < 8; k++)
            {
                if (ival % 2 == 1)
                    v[j] = "1" + v[j];
                else
                    v[j] = "0" + v[j];
                ival /= 2;
            }
        }
        reverse(v.begin(), v.end());
        addTask(v);

        v.clear();
        v.push_back("0");
        addTask(v);
    }
}

//! Update display
void ExampleDisplayTask::execute()
{
    if (m_mesh.getNodeIndex().first == 0)
    {
        /*for (int i = 0; i < 10; i++)
            if (img.size() < 20)
                addTask();*/
    }
    if (!img.size())
        return;
   //MY_DEBUG_PRINTLN(img.size());
   int cs = m_mesh.getNodeIndex().first * 8;
   if (m_mesh.getNodeIndex().first == 0)
    m_x = 0;
  int tcur = m_mesh.millis();
   while (img.size() >= 2)
   {
       if (gv (process (img[1]).back()) > tcur) break;
       img.erase(img.begin());
   }
   istart++;
   vector <string> v = process (img[0]);
   v.pop_back();

   int sc = scale(v);
   if (sc == 0) sc = 1;
   m_lmd.clear();
   for (int i = 0; i < 32; i++)
   {
        for (int j = 0; j < 8; j++)
        {
            int x = i / sc, y = (j + cs) / sc;
            if (x < v.size() && y < v[x].length() && v[x][y] == '1')
               display (i, j);
        }
   }

   //MY_DEBUG_PRINTLN(ct);
   //long now = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
   //MY_DEBUG_PRINTLN(second());
   //MY_DEBUG_PRINTLN(now);
   if (cs == 0)
    display (31, 7);
   else
    display (30, 7);
   /*for (int y = 0; y < 8; y++)
    display (m_x, y);*/
   m_lmd.display();
}

void ExampleDisplayTask::updateImage(String& msg)
{
    //img.clear();
    //img.push_back(msg);

   // trim here, lol
}

void ExampleDisplayTask::receivedCb(Facilities::MeshNetwork::NodeId nodeId, String& msg)
{
   //MY_DEBUG_PRINTLN("Received data in ExampleDisplayTask");
   //MY_DEBUG_PRINTLN(msg);

   if (msg[0] == 't')
   {
       int cm = 0;
       for (int i = 1; i < msg.length(); i++)
        cm = 10 * cm + (msg[i] - '0');
        m_x = min (m_x, (int) millis() - cm);
       return;
   }

   int cloc = 0;
   img.clear();
   String s = "";
   while (cloc < msg.length())
   {
       if (msg[cloc] == ',')
       {
           img.push_back(s);
           s = "";
       }
       else
       {
           s += msg[cloc];
       }
       cloc++;
   }
   istart = 0;
   execute();
}

} // namespace Tasks
