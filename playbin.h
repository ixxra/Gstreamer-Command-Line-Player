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


#ifndef PLAYBIN_H
#define PLAYBIN_H

#include "playlist.h"
#include <gst/gst.h>
#include <gst/gstbus.h>
#include <string>

class Playbin
{

public:
Playbin();
virtual ~Playbin();

void play() { gst_element_set_state(playbin, GST_STATE_PLAYING); }
void stop() { gst_element_set_state(playbin, GST_STATE_NULL); }
void pause(){ gst_element_set_state(playbin, GST_STATE_PAUSED); }

void seek(int sec);
void setVolume(double delta);

GstState state();
void setUri(const std::string& uri);
void setBusCallback(GstBusFunc callback, gpointer data);

private:
  GstElement* playbin;
};

#endif // PLAYBIN_H
