import * as React from 'react';
import 'reflect-metadata'
import { StyleSheet, View, Text, FlatList } from 'react-native';
import { createDb } from './Database';
import type { User } from './model/User';

export default function App() {
  let [users, setUsers] = React.useState<User[]>([])

  React.useEffect(() => {
    createDb().then((users) => {
      setUsers(users)
    })
  }, [])

  return (
    <View style={styles.container}>
      <FlatList
        data={users}
        renderItem={(info: any) => {
          return (
            <View>
              <Text>{info.item.name} - {info.item.age}</Text>
            </View>
          )
        }}
        keyExtractor={(item: any) => item.id}
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
