/*
  response state_getMetadata
*/

struct ArgItem
{
  char key[DEFAULT_SIZE];
  char value[DEFAULT_SIZE][MAX_NAME_SIZE];
}

struct EventArg
{
  char name[DEFAULT_SIZE];
  ArgItem argItem[DEFAULT_SIZE];
}

struct OuterEvent
{
  char[DEFAULT_SIZE] name;
  EventArg[DEFAULT_SIZE] eventArg;
}

struct Function
{
  char name[DEFAULT_SIZE];
  char modifier[DEFAULT_SIZE];
  ArgItem type[DEFAULT_SIZE];
  char fallback[DEFAULT_SIZE];
  char documentation[DEFAULT_SIZE][MAX_NAME_SIZE];
}

struct ModuleContent
{
  char name[DEFAULT_SIZE];
  ArgItem calls[DEFAULT_SIZE]; // maybe wrong implementation
}

struct Storage
{
  char prefix[DEFAULT_SIZE];
  Function function[DEFAULT_SIZE];
}

struct Module
{
  char prefix[DEFAULT_SIZE];
  ModuleContent module;
  Storage storage;
}

struct Call
{
  char name[DEFAULT_SIZE];
  char prefix[DEFAULT_SIZE];
  int index;
}

struct OuterDispatch
{
  char name[DEFAULT_SIZE];
  Call calls[DEFAULT_SIZE];
}

struct Metadata
{
  char outerEventsName[DEFAULT_SIZE];
  OuterEvent outerEvents[DEFAULT_SIZE];
  Module modules;
  OuterDispatch outerDispatch;
}

struct SystemPeers
{
  PeerItem peers[DEFAULT_SIZE];
};
