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


#include "playlist.h"
#include <algorithm>

using namespace std;

Playlist::Playlist()
{

}

Playlist::~Playlist()
{
  for (list<Track*>::iterator it = playlist.begin(); it != playlist.end(); ++it){
    delete *it;
  }
}

void Playlist::append(Track* track)
{
  playlist.push_back(track);
}

void Playlist::append(TrackList& other)
{
  playlist.splice(playlist.end(), other);
}

void Playlist::clear()
{
  playlist.clear();
}

void Playlist::remove(Track* track)
{
  TrackList::iterator it = find(playlist.begin(), playlist.end(), track);
  delete *it;
  playlist.remove(track);
}

int Playlist::indexOf(const Track* track)
{
  int idx = 0;
  
  for (TrackList::iterator it = playlist.begin(); it != playlist.end(); ++it){
    if (*it == track) break;
    ++idx;
  }
  
  return idx == playlist.size()? -1 : idx;
}

Track* Playlist::trackAbove(const Track* track)
{
  
}

Track* Playlist::trackAt(int pos)
{
  if (pos < 0 || pos >= playlist.size()) return 0;
  
  list<Track*>::iterator it(playlist.begin());
  int idx = 0;
  while(idx <= pos) ++it;
  return *it;
}

Track* Playlist::trackBelow(const Track* track)
{
  TrackList::iterator it = find(playlist.begin(), playlist.end(), track);
  if (it == playlist.end()) return 0;
  else {
    ++it;
    return *it;
  }
}


