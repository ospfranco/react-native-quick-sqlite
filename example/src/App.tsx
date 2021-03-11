import * as React from 'react';

import { StyleSheet, View, Text, FlatList } from 'react-native';
// import { openDb, execSQL, closeDb, deleteDb, asyncExecSQL } from 'react-native-sequel';
import sqlite from 'react-native-sequel';
import { createConnection } from 'typeorm/browser';


export default function App() {
  const [rows, setRows] = React.useState<any[]>([])
  
  React.useEffect(() => {
    
    // console.warn('sqlite', sqlite)
    createConnection({
      type: 'react-native',
      database: 'test',
      location: 'default',
      logging: ['error', 'query', 'schema'],
      // entities
    })
    
    // openDb(`sample.sqlite`);
    
    // execSQL(`CREATE TABLE PEOPLE (ID TEXT PRIMARY KEY NOT NULL, NAME TEXT NOT NULL);`);

    // const rows = execSQL(`SELECT * FROM 'PEOPLE';`);

    // if(rows.length === 0) {
    //   for(let ii = 0; ii < 10000; ii++) {
    //     let sql = `INSERT INTO PEOPLE ('ID', 'NAME') VALUES ('${ii}', 'value of ${ii}')`;
    //     execSQL(sql);
    //   }
    // }

    // closeDb('sample.sqlite');

    // deleteDb('sample.sqlite');

    // // execSQL(`DROP TABLE 'PEOPLE';`);

    // setRows(rows);
    // asyncExecSQL(`sample.sqlite`, `SELECT * FROM 'PEOPLE';`).then(({error, result}) => {
    //   if(error) {
    //     console.warn('something went wrong executing query', error.toString())
    //   } else {
    //     setRows(result!);
    //   }
    // })
  }, []);

  return (
    <View style={styles.container}>
      <FlatList
        data={rows}
        renderItem={(info: any) => {
          return (
            <View>
              <Text>{info.item.ID} - {info.item.NAME}</Text>
            </View>
          )
        }}
        keyExtractor={(item: any) => item.ID}
      />

    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    paddingTop: 40,
  },
  box: {
    width: 60,
    height: 60,
    marginVertical: 20,
  },
});
