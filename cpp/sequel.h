#include <vector>
#include <jsi/jsilib.h>
#include <jsi/jsi.h>

using namespace std;
using namespace facebook;

bool sequel_open(string const &dbName);

bool sequel_close(string const &dbName);

bool sequel_delete(string const $dbName);

vector<jsi::Object> sequel_execute(jsi::Runtime &runtime, string const &query);
