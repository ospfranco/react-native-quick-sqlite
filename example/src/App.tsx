import * as React from 'react';

import { StyleSheet, View, Text, FlatList } from 'react-native';
import { initDb } from 'react-native-sequel';

export default function App() {
  const [rows, setRows] = React.useState([])

  React.useEffect(() => {
    // setIsDbOpen(openDb("sequel.db"))
    const res = initDb()
    setRows(res);
  }, []);

  return (
    <View style={styles.container}>
      <FlatList
        data={rows}
        renderItem={(info: any) => {
          return (
            <View key={info.key}>
              <Text>{info.item.ID} - {info.item.NAME}</Text>
            </View>
          )
        }}
      />
      {/* <Text>is db open: {isDbOpen.toString()}</Text> */}
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
  box: {
    width: 60,
    height: 60,
    marginVertical: 20,
  },
});
