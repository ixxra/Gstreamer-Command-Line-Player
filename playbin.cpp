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


#include "playbin.h"
#include <glib-object.h>

Playbin::Playbin() : playbin(0)
{
  playbin = gst_element_factory_make("playbin2", "playbin");
  g_assert(playbin);
}

Playbin::~Playbin()
{
  gst_element_set_state(playbin, GST_STATE_NULL);
  gst_object_unref(playbin);
}

void Playbin::setBusCallback(GstBusFunc callback, gpointer data)
{
  GstBus* bus = gst_element_get_bus(playbin);
  gst_bus_add_watch(bus, callback, data);
  gst_object_unref(bus);
}

void Playbin::setUri(const std::string& uri)
{
  gst_element_set_state(playbin, GST_STATE_NULL);
  g_object_set(G_OBJECT(playbin), "uri", uri.c_str(), NULL);
}

GstState Playbin::state()
{
  GstState state;
  gst_element_get_state(playbin, &state, NULL, -1);
  return state;
}

void Playbin::seek(int sec)
{
  gint64 pos;
  GstFormat format = GST_FORMAT_TIME;
   
  if (gst_element_query_position(playbin, &format, &pos)){
    if (format != GST_FORMAT_TIME){
      g_warning("Avoiding seek... GST_FORMAT other than time!. This is a bug in gplay!\n");
      return;
    }
    pos += sec * GST_SECOND;
    
    gst_element_seek_simple(playbin, 
			    GST_FORMAT_TIME, 
			    GST_SEEK_FLAG_FLUSH, 
			    pos);
  }
  else
    g_print("Not a seekable stream!\n");
}

void Playbin::setVolume(gdouble delta)
{
  gdouble vol;
  g_object_get(G_OBJECT(playbin), "volume", &vol, NULL);
  vol += delta;
  g_object_set(G_OBJECT(playbin), "volume", vol, NULL);
  g_print("volume set to %.0f%%\n", 100 * vol);
}


















