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


#include "commandlineparser.h"
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <gst/gst.h>
#include <string>
#include <cstring>

static
gchar* uri_from_local(const char* name)
{
  char* path = canonicalize_file_name(name);
  gchar* uri = g_filename_to_uri(path, NULL, NULL);
  free(path);
  
  return uri;
}

static inline
void append_file(Playlist& playlist, const char* file)
{
  printf("appending %s\n", file);
  Track* track = new Track;  
  if (gst_uri_is_valid(file))
    track->uri = file;  
  else{
    gchar* uri = uri_from_local(file);
    track->uri = uri;
    g_free(uri);
  }
  
  playlist.push_back(track);
}

void parse_plain_playlist(Playlist& playlist, const char* filename)
{
  printf("parsing playlist %s\n", filename);
  GIOChannel* channel =  g_io_channel_new_file(filename, "r", NULL);
  gchar* line = 0;
  while (g_io_channel_read_line(channel, &line, 0, 0, 0) != G_IO_STATUS_EOF){
    g_strstrip(line);
    
    if (strlen(line) > 0)
      append_file(playlist, line);
    
    g_free(line);
    line = 0;
  }
  g_io_channel_unref(channel);
}

void parse_command_line(int argc, char* argv[], Playlist& playlist)
{
  for (int i = 1; i < argc; i++){
    const char* key = argv[i];
    const char* arg = 0;
    if (key[0] == '-'){
      if (strlen(key) > 2){
	arg = key + 2;
      }
      else {
	if (i + 1 == argc || argv[i + 1][0] == '-'){
	  printf("error parsing token %d\n", i);
	  continue;
	}
	else{
	  ++i;
	  arg = argv[i];
	}
      }
      switch(key[1]){
	case 'l':
	  parse_plain_playlist(playlist, arg);
	  break;
	  
	default:
	{
	  printf("unrecognized option -%c\n", key[1]);
	  append_file(playlist, arg);
	  break;
	}
      }
    }
    else {
      append_file(playlist, argv[i]);
    }
  }
}

