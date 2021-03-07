import * as React from 'react';

import { StyleSheet, View, Text, FlatList } from 'react-native';
import { openDb, execSQL, closeDb, deleteDb, asyncExecSQL } from 'react-native-sequel';

export default function App() {
  const [rows, setRows] = React.useState([])
  const [finalTime, setFinalTime] = React.useState(0);

  React.useEffect(() => {
    
    openDb(`sample.sqlite`);
    
    const initTime = new Date();
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
    asyncExecSQL(`sample.sqlite`, `SELECT * FROM 'PEOPLE';`).then((res) => {
      setRows(res);
      
      setFinalTime(new Date().getTime() - initTime.getTime());
    })
  }, []);

  return (
    <View style={styles.container}>
      <Text style={{fontWeight: 'bold'}}>Loaded {rows.length} items in {finalTime} ms</Text>
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
