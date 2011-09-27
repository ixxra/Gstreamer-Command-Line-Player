#include "commandlineparser.h"
#include "playlist.h"
#include "gplay.h"
#include <gst/gst.h>
#include <termios.h>


static const char* help = "Usage: gplay  [-l playlist|file] ...\n";


int main(int argc, char **argv) {
  if (argc == 1){
    g_print(help);
    return 0;
  }
  
  gst_init(&argc, &argv);
  
  struct termios termios_p;
  tcgetattr(STDIN_FILENO, &termios_p);
  termios_p.c_lflag &= ~(ICANON|ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &termios_p);
  
  gPlay gplay;
  Playlist playlist;
  parse_command_line(argc, argv, playlist);
  
  gplay.setPlaylist(playlist);
  gplay.play();
  
  GMainLoop* loop = g_main_loop_new(NULL, false);
  g_main_loop_run(loop);
  
  return 0;
}
