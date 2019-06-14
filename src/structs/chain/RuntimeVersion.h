/*
  response chain_getRuntimeVersion
*/

struct ApiItem
{
  char num[DEFAULT_SIZE];
  int id;
}

struct RuntimeVersion
{
  ApiItem api[DEFAULT_SIZE];
  uint authoritingVersion;
  char implName[MAX_NAME_SIZE];
  int implVersion;
  char specName[MAX_NAME_SIZE];
  int specVersion;
};
