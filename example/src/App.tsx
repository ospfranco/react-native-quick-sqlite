import * as React from 'react';

import { StyleSheet, View, Text, FlatList } from 'react-native';
import { openDb, execSQL, closeDb } from 'react-native-sequel';

export default function App() {
  const [rows, setRows] = React.useState([])
  const [finalTime, setFinalTime] = React.useState(0);

  React.useEffect(() => {
    const initTime = new Date();

    openDb('sample.sqlite');

    // execSQL(`DROP TABLE 'PEOPLE';`);

    // execSQL(`CREATE TABLE PEOPLE (ID TEXT PRIMARY KEY NOT NULL, NAME TEXT NOT NULL);`);

    // for(let ii = 0; ii < 10000; ii++) {
    //   let sql = `INSERT INTO PEOPLE ('ID', 'NAME') VALUES ('${ii}', 'value of ${ii}')`;
    //   execSQL(sql);
    // }

    const rows = execSQL(`SELECT * FROM 'PEOPLE';`);
    
    setFinalTime(new Date().getTime() - initTime.getTime());
    setRows(rows);
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
