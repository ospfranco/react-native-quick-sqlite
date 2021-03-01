import * as React from 'react';

import { StyleSheet, View, Text, FlatList } from 'react-native';
import { initDb } from 'react-native-sequel';

export default function App() {
  const [rows, setRows] = React.useState([])
  const [finalTime, setFinalTime] = React.useState(0);

  React.useEffect(() => {
    const initTime = new Date();
    const res = initDb()

    setRows(res);
    setFinalTime(new Date().getTime() - initTime.getTime());
  }, []);

  return (
    <View style={styles.container}>
      <Text>Total time: {finalTime} ms</Text>
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
    paddingTop: 40,
    alignItems: 'center',
    justifyContent: 'center',
  },
  box: {
    width: 60,
    height: 60,
    marginVertical: 20,
  },
});
