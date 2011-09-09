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


#include "gplay.h"
#include "playlist.h"
#include <string>


void track_from_tag(const GstTagList* tag, Track* track)
{
  gchar* value = 0;
  
  if (gst_tag_list_get_string(tag, GST_TAG_TITLE, &value)){
    track->title = value;
    g_free(value);
  }
  
  if (gst_tag_list_get_string(tag, GST_TAG_ARTIST, &value)){
    track->artist = value;
    g_free(value);
  }
  
  if (gst_tag_list_get_string(tag, GST_TAG_ALBUM, &value)){
    track->album = value;
    g_free(value);
  }
}

gboolean bus_callback(GstBus* sender, GstMessage* message, void* data)
{
  gPlay* gplay = reinterpret_cast<gPlay*> (data);
  
  switch (GST_MESSAGE_TYPE (message)) {
    case GST_MESSAGE_STATE_CHANGED:
    {
      GstState newState;
      gst_message_parse_state_changed(message, NULL, &newState, NULL);
      
      std::string message_name(GST_MESSAGE_SRC_NAME(message));//TODO: Avoid this copy using glib
      
      if (message_name.compare("playbin") == 0){
	gplay->on_state_changed(newState);
      }
    }
      break;
      
    case GST_MESSAGE_TAG:
    {
      GstTagList* tag_list = 0;
      gst_message_parse_tag(message, &tag_list);
      Track t;
      track_from_tag(tag_list, &t);
      gplay->on_tag_found(t);
      gst_tag_list_free(tag_list);
    }
      break;
      
    case GST_MESSAGE_EOS:
      gplay->on_eos();
      break;
      
    case GST_MESSAGE_STREAM_STATUS:
      GstStreamStatusType message_type;
      gst_message_parse_stream_status(message, &message_type, NULL);
      g_print("Stream status: %d\n", message_type);
      break;
      
    default:
      g_print("Message from %s: %s\n", GST_MESSAGE_SRC_NAME(message), gst_message_type_get_name(GST_MESSAGE_TYPE(message)));
      break;
  }
  
  //TODO: Should I dispose message?
  return true;
}

gPlay::gPlay()
{
  playbin.setBusCallback(bus_callback, this);
  keybinder.setPlayer(this);
}

gPlay::~gPlay()
{

}

void gPlay::setPlaylist(Playlist& other)
{
  playlist.clear();
  playlist.splice(playlist.end(), other);
  position = playlist.begin();
  playbin.setUri((*position)->uri);
}

void gPlay::next()
{
  playbin.stop();
  ++position;
  
  if (position == playlist.end()){
    g_print("End of playlist\n");
    return;
  }
  
  playbin.setUri((*position)->uri);
  playbin.play();
}

void gPlay::prev()
{
  if (position == playlist.begin()){
      g_print("No previous item\n");
      return;
  }
  
  playbin.stop();
  --position;
  playbin.setUri((*position)->uri);
  playbin.play();
}

void gPlay::on_eos()
{
  next();
}

void gPlay::on_state_changed(GstState newState)
{
  switch (newState){
    case GST_STATE_PAUSED:
      g_print("PAUSED\n");
      break;
      
    case GST_STATE_PLAYING:
      g_print("PLAYING\n");
      break;
    case GST_STATE_READY:
    case  GST_STATE_NULL:
      g_print("STOPED\n");
      break;
      
    default:
      break;
  }
}

void gPlay::on_tag_found(const Track& tag)
{
  if (tag.title.length() == 0 && tag.artist.length() == 0 && tag.album.length() == 0)
    return;
 
  g_message("Tags found:");
  Track* track = *position;
  
  if (tag.title.length() != 0) track->title = tag.title;
  
  if (tag.artist.length() != 0) track->artist = tag.artist;
  
  if (tag.album.length() != 0) track->album = tag.album;
  
  
  g_print(" Title: %s \n"
          "Artist: %s \n"
	  " Album: %s \n"
	  "   Uri: %s \n", 
          track->title.c_str(), track->artist.c_str(), 
	  track->album.c_str(), track->uri.c_str());
}
