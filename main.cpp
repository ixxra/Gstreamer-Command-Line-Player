#include "playbin.h"
#include "playlist.h"
#include "keysdelegate.h"
#include <gst/gst.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <curses.h>
#include <termios.h>
#include <cstdio>
#include "gplay.h"


using namespace std;

static
string uri_from_local(const char* name)
{
  char* path = canonicalize_file_name(name);
  gchar* local_uri = g_filename_to_uri(path, NULL, NULL);
  string uri(local_uri);
  g_free(local_uri);
  free(path);
  
  return uri;
}

int main(int argc, char **argv) {
  if (argc == 1){
    cout << "Usage: playbin <file1>..." << endl;
    return 0;
  }
  
  gst_init(&argc, &argv);
  
  struct termios termios_p;
  tcgetattr(STDIN_FILENO, &termios_p);
  termios_p.c_lflag &= ~(ICANON|ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &termios_p);
  
  gPlay gplay;
  Playlist playlist;
  
  for (int i = 1; i < argc; i++){
    string source = uri_from_local(argv[i]);
    Track* track = new Track;
    track->uri = source;
    playlist.push_back(track);
  }
  
  gplay.setPlaylist(playlist);
  gplay.play();
  
  GMainLoop* loop = g_main_loop_new(NULL, false);
  g_main_loop_run(loop);
  
  return 0;
}
