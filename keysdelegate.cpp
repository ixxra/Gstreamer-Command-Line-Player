/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "keysdelegate.h"
#include <glib.h>
#include <cstdio>
#include "gplay.h"

using namespace std;

 gboolean on_key_press(GIOChannel *source, GIOCondition condition, gpointer data)
 {
   if (condition != G_IO_IN) return true;
   
   gPlay* player = reinterpret_cast<gPlay*>(data);
   
   char key;
   g_io_channel_read_chars(source, &key, 1, NULL, NULL);
   
   switch (key){
     case 'n': 
       player->next();
       break;
       
     case 'p':
       player->prev();
       break;
       
     case 'z':
       if (player->state() == GST_STATE_PLAYING)
	player->pause();
       else if (player->state() == GST_STATE_PAUSED)
	player->play();
       break;
       
     case 'x':
       player->stop();
       break;
       
     case 'c':
       player->play();
       break;
       
     case 'f':
       player->seek(5);
       break;
       
     case 'b':
       player->seek(-5);
       break;
       
     case 'u':
       player->setVolume(+0.05);
       break;
       
     case 'd':
       player->setVolume(-0.05);
       break;
       
     default:
       break;
   }
   
   return true;
 }

KeysDelegate::KeysDelegate() : stdin_channel(0)
{
}

KeysDelegate::~KeysDelegate()
{
  g_io_channel_close(stdin_channel);
  g_io_channel_unref(stdin_channel);
}

void KeysDelegate::setPlayer(gPlay* player)
{
  if (!stdin_channel)
    stdin_channel = g_io_channel_unix_new(STDIN_FILENO);
  
  g_io_add_watch(stdin_channel, G_IO_IN, on_key_press, player);
}
