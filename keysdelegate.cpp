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
#include "playlist.h"
#include <glib.h>
#include <cstdio>
#include <termios.h>
#include "gplay.h"

using namespace std;



void print_playlist(gPlay* player)
{
  int ctr = 1;
  
  Playlist playlist = player->getPlaylist();
  Playlist::const_iterator playlistPosition = player->playlistPosition();
  
  g_print("============= PLAYLIST =============\n");
  for ( Playlist::const_iterator pos = playlist.begin(); pos != playlist.end(); ++pos){
    const Track* t = *pos;
    if (t->title.length() == 0){
      gchar* filename = g_filename_from_uri(t->uri.c_str(), NULL, NULL);
      gchar* basename = g_path_get_basename(filename);
      
      if ( g_strcmp0((*pos)->uri.c_str(), (*playlistPosition)->uri.c_str()) == 0 ){
	g_print("\033[31m%02d - %s by %s\033[39m\n", ctr, basename, t->artist.c_str());
      }
      
      else
	g_print("%02d - %s by %s\n", ctr, basename, t->artist.c_str());
      
      g_free(basename);
      g_free(filename);
      
    } 
    else{
      if ( g_strcmp0((*pos)->uri.c_str(), (*playlistPosition)->uri.c_str()) == 0){
	g_print("\033[31m%02d - %s by %s\n\033[39m", ctr, t->title.c_str(), t->artist.c_str());
      }
      else
	g_print("%02d - %s by %s\n", ctr, t->title.c_str(), t->artist.c_str());
    }
    ++ctr;
  }
  g_print("====================================\n");
}

static
void jump_to(gPlay* gplay)
{
  g_print("Jump to: ");
  
  struct termios termios_p;
  tcgetattr(STDIN_FILENO, &termios_p);
  termios_p.c_lflag |= (ICANON|ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &termios_p);
  
  //TODO: Disconnect key reading here!
  int pos = -1;
  scanf("%d", &pos);
  g_print("scanned position: %d\n", pos);
  --pos;
  
  if (pos >= 0 && pos < gplay->getPlaylist().size() ){
    gplay->stop();
    gplay->playlistPosition() = gplay->getPlaylist().begin();
    for (int i = 0; i < pos; i++)
      ++(gplay->playlistPosition());
    gplay->play();
  } else 
    g_print("invalid position\n");
  
  termios_p.c_lflag &= ~(ICANON|ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &termios_p);
}

 gboolean on_key_press(GIOChannel *source, GIOCondition condition, gpointer data)
 {
   if (condition != G_IO_IN) return true;
   
   gPlay* player = reinterpret_cast<gPlay*>(data);
   
   char key;
   g_io_channel_read_chars(source, &key, 1, NULL, NULL);
   g_io_channel_flush(source, NULL);
  
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
       player->setVolume(+0.01);
       break;
       
     case 'U':
       player->setVolume(+0.05);
       break;
       
     case 'd':
       player->setVolume(-0.01);
       break;
       
     case 'D':
       player->setVolume(-0.05);
       break;
       
     case 'l':
       print_playlist(player);
       break;
       
     case 'j':
       jump_to(player);
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

