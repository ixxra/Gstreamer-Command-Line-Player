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


#include "taglistdelegate.h"
#include <gst/gst.h>
#include <string.h>

TagListDelegate::TagListDelegate() : playlist(0), parser(0), dec(0)
{

}

TagListDelegate::~TagListDelegate()
{
  gst_element_set_state(parser, GST_STATE_NULL);
  gst_object_unref(parser);
}

static void
print_one_tag (const GstTagList * list, const gchar * tag, gpointer user_data)
{
  //int i, num;
  
  Playlist::iterator* pos = reinterpret_cast<Playlist::iterator*>(user_data);
  
  gchar* value = 0;
  
  if (g_strcmp0(tag, GST_TAG_TITLE) == 0){
    gst_tag_list_get_string(list, tag, &value);
    (*(*pos))->title = value;
    g_free(value);
  }
  else if (g_strcmp0(tag, GST_TAG_ARTIST) == 0){
    gst_tag_list_get_string(list, tag, &value);
    (*(*pos))->artist = value;
    g_free(value);
  }
  else if (g_strcmp0(tag, GST_TAG_ALBUM) == 0){
    gst_tag_list_get_string(list, tag, &value);
    (*(*pos))->album = value;
    g_free(value);
  }
  
/*
  num = gst_tag_list_get_tag_size (list, tag);
  for (i = 0; i < num; ++i) {
    const GValue *val;
*/
    /* Note: when looking for specific tags, use the g_tag_list_get_xyz() API,
     * we only use the GValue approach here because it is more generic */
    //val = gst_tag_list_get_value_index (list, tag, i);
  /*  if (G_VALUE_HOLDS_STRING (val)) {
      g_print ("\t%20s : %s\n", tag, g_value_get_string (val));
    } else if (G_VALUE_HOLDS_UINT (val)) {
      g_print ("\t%20s : %u\n", tag, g_value_get_uint (val));
    } else if (G_VALUE_HOLDS_DOUBLE (val)) {
      g_print ("\t%20s : %g\n", tag, g_value_get_double (val));
    } else if (G_VALUE_HOLDS_BOOLEAN (val)) {
      g_print ("\t%20s : %s\n", tag,
          (g_value_get_boolean (val)) ? "true" : "false");
    } else if (GST_VALUE_HOLDS_BUFFER (val)) {
      g_print ("\t%20s : buffer of size %u\n", tag,
          GST_BUFFER_SIZE (gst_value_get_buffer (val)));
    } else if (GST_VALUE_HOLDS_DATE (val)) {
      g_print ("\t%20s : date (year=%u,...)\n", tag,
          g_date_get_year (gst_value_get_date (val)));
    } else {
      g_print ("\t%20s : tag of type '%s'\n", tag, G_VALUE_TYPE_NAME (val));
    }
  }*/
}

static 
void on_new_pad (GstElement * dec, GstPad * pad, GstElement * fakesink)
{
  GstPad *sinkpad;

  sinkpad = gst_element_get_static_pad (fakesink, "sink");
  if (!gst_pad_is_linked (sinkpad)) {
    if (gst_pad_link (pad, sinkpad) != GST_PAD_LINK_OK)
      g_error ("Failed to link pads!");
  }
  gst_object_unref (sinkpad);
}

void TagListDelegate::setPlaylist(Playlist* playlist)
{
  this->playlist = playlist;
  
  if (!parser){ 
    parser = gst_pipeline_new ("pipeline");
    dec = gst_element_factory_make ("uridecodebin", NULL);
    gst_bin_add (GST_BIN (parser), dec);

    GstElement* sink = gst_element_factory_make ("fakesink", NULL);
    gst_bin_add (GST_BIN (parser), sink);

    g_signal_connect (dec, "pad-added", G_CALLBACK (on_new_pad), sink);
    gst_element_set_state (parser, GST_STATE_PAUSED);
  }
}

void TagListDelegate::parseTags()
{
  GstBus* bus = gst_element_get_bus(parser);
  gst_bus_add_watch(bus, GstBusFunc(bus_callback), this);
  gst_object_unref(bus);
  
  pos = playlist->begin();
  g_object_set(G_OBJECT(dec), "uri", (*pos)->uri.c_str(), NULL);
  gst_element_set_state(parser, GST_STATE_PLAYING);
}

gboolean TagListDelegate::bus_callback(GstBus* sender, GstMessage* msg, void* data)
{
  TagListDelegate* delegate = reinterpret_cast<TagListDelegate*>(data);
  
  switch (GST_MESSAGE_TYPE(msg)){
    case GST_MESSAGE_TAG:
    {
      GstTagList* tags;
      gchar* uri;
      g_object_get(delegate->dec, "uri", &uri, NULL);
      gst_message_parse_tag(msg, &tags);
      g_free(uri);
      gst_tag_list_foreach (tags, print_one_tag, &(delegate->pos));    
      gst_tag_list_free(tags);
    }
      break;
      
    case GST_MESSAGE_ASYNC_DONE:
      ++(delegate->pos);
      if (delegate->pos != delegate->playlist->end()){
	gst_element_set_state(delegate->parser, GST_STATE_NULL);
	g_object_set(G_OBJECT(delegate->dec), "uri", (*delegate->pos)->uri.c_str(), NULL);
	gst_element_set_state(delegate->parser, GST_STATE_PLAYING);
      }
      
      else {
	g_print("TagsDelegate: End of tagging. Stopping!");
	return false;
      }
      break;
  }
  
  return true;
}





