#include <string>
#include <vector>
#include <map>

using namespace std;

bool sequel_open(string const& dbName);

void sequel_execute(string const& query);

std::vector<std::map<string, string> > sequel_init();