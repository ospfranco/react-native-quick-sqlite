import 'reflect-metadata';
import * as React from 'react';
import {
  StyleSheet,
  View,
  Text,
  FlatList,
  ListRenderItemInfo,
} from 'react-native';
import { createDb } from './Database';
import type { User } from './model/User';

export default function App() {
  let [users, setUsers] = React.useState<User[]>([]);

  React.useEffect(() => {
    createDb().then((users) => {
      setUsers(users);
    });
  }, []);

  return (
    <View style={styles.container}>
      <FlatList
        data={users}
        renderItem={(info: ListRenderItemInfo<User>) => {
          return (
            <View
              style={{
                marginHorizontal: 10,
                marginVertical: 4,
                borderRadius: 10,
                padding: 10,
                backgroundColor: 'white',
              }}
            >
              <Text style={{ fontWeight: 'bold' }}>Name</Text>
              <Text>{info.item.name}</Text>
              <Text style={{ fontWeight: 'bold', marginTop: 10 }}>Age</Text>
              <Text>{info.item.age}</Text>
              <Text style={{ fontWeight: 'bold', marginTop: 10 }}>
                Favorite Book
              </Text>
              <Text>{info.item.favoriteBook.title}</Text>
            </View>
          );
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
    backgroundColor: '#EEE',
  },
  box: {
    width: 60,
    height: 60,
    marginVertical: 20,
  },
});
