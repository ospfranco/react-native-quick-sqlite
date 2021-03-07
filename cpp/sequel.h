#include <vector>
#include <jsi/jsilib.h>
#include <jsi/jsi.h>

using namespace std;
using namespace facebook;

bool sequel_open(string const &dbName);

bool sequel_remove(string const &dbName);

bool sequel_close(string const &dbName);

vector<jsi::Object> sequel_execute(jsi::Runtime &rt, string const &query);

void sequel_execute_async(jsi::Runtime &rt, const jsi::Value &resolve);
