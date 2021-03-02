#include <vector>

#include <jsi/jsilib.h>
#include <jsi/jsi.h>

using namespace std;
using namespace facebook;

bool sequel_open(string const& dbName);

void sequel_execute(string const& query);

std::vector<jsi::Object> sequel_init(jsi::Runtime& runtime);
