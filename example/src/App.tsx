import * as React from 'react';

import { StyleSheet, View, Text, FlatList } from 'react-native';



export default function App() {
  const rows: any[] = []

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
