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


#ifndef GPLAY_H
#define GPLAY_H

#include "playbin.h"
#include "keysdelegate.h"
#include "playlist.h"
#include "taglistdelegate.h"

class gPlay
{

public:
gPlay();
virtual ~gPlay();
void setPlaylist(Playlist& other);

void play() {playbin.play();}
void pause(){playbin.pause();}
void stop() {playbin.stop();}

void next();
void prev();

void seek(int sec){playbin.seek(sec);}
void setVolume(gdouble delta){playbin.setVolume(delta);}

GstState state(){ return playbin.state();}
Playlist& getPlaylist() { return playlist; }
Playlist::iterator& playlistPosition() { return position; }

private:
  Playbin playbin;
  Playlist playlist;
  Playlist::iterator position;
  KeysDelegate keybinder;
  TagListDelegate tagsdelegate;
  
  void on_state_changed(GstState newState);
  void on_eos();
  void on_tag_found(const Track& tag);
  
friend gboolean bus_callback(GstBus* sender, GstMessage* message, void* data);
};

#endif // GPLAY_H
