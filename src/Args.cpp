#include "core.h"
#include "Args.h"

namespace game
{

  Args::Args(int argc, char** argv) :
    argc(argc),
    argv(argv)
  {
    for (int i = 1; i < argc; i++)
    {
      if (!strcmp(argv[i], "-server"))
        mode = Mode::Server;
      else if (!strcmp(argv[i], "-test"))
        mode = Mode::Test;
      else if (!strcmp(argv[i], "-host"))
        serverConfig.host = argv[++i];
      else if (!strcmp(argv[i], "-port"))
        serverConfig.port = (unsigned short)atoi(argv[++i]);
      else if (!strcmp(argv[i], "-password"))
        serverConfig.password = argv[++i];
      else if (!strcmp(argv[i], "-players"))
        serverConfig.maxPlayers = atoi(argv[++i]);
    }
  }

}
