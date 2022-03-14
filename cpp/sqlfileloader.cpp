/**
 * SQL File Loader implementation
*/
#include "sqlfileloader.h"
#include <iostream>
#include <fstream>

using namespace std;

SequelBatchOperationResult importSQLFile(string dbName, string fileLocation)
{
  string line;
  ifstream sqFile(fileLocation);
  if (sqFile.is_open())
  {
    try
    {
      int affectedRows = 0;
      int commands = 0;
      sequel_execute_literal_update(dbName, "BEGIN EXCLUSIVE TRANSACTION");
      while (std::getline(sqFile, line, '\n'))
      {
        if (!line.empty())
        {
          SequelLiteralUpdateResult result = sequel_execute_literal_update(dbName, line);
          if (result.type == SequelResultError)
          {
            sequel_execute_literal_update(dbName, "ROLLBACK");
            sqFile.close();
            return {SequelResultError, result.message, 0, commands};
          }
          else
          {
            affectedRows += result.affectedRows;
            commands++;
          }
        }
      }
      sqFile.close();
      sequel_execute_literal_update(dbName, "COMMIT");
      return {SequelResultOk, "", affectedRows, commands};
    }
    catch (...)
    {
      sqFile.close();
      sequel_execute_literal_update(dbName, "ROLLBACK");
      return {SequelResultError, "[react-native-quick-sqlite][loadSQLFile] Unexpected error, transaction was rolledback", 0, 0};
    }
  }
  else
  {
    return {SequelResultError, "[react-native-quick-sqlite][loadSQLFile] Could not open file", 0, 0};
  }
}
