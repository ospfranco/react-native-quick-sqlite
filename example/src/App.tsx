import 'reflect-metadata';
import React from 'react';
import {
  StyleSheet,
  Image,
  View,
  Text,
  FlatList,
  ListRenderItemInfo,
  Button,
} from 'react-native';
// Swap imports to test the typeORM version
import {
  lowLevelInit,
  queryUsers,
  testInsert,
  testTransaction,
  typeORMGetBooks,
  typeORMInit,
  executeFailingTypeORMQuery,
} from './Database';
import type { User } from './model/User';
import { Buffer } from 'buffer';

export default function App() {
  let [users, setUsers] = React.useState<User[]>([]);

  React.useEffect(() => {
    lowLevelInit();
    const users = queryUsers();
    // console.warn('db users', users);
    setUsers(users);
    typeORMInit().then(() => {
      console.warn('typeorm initialized!');
      typeORMGetBooks().then((books) => {
        console.warn('typeORM books', books);
      });
    });
  }, []);

  return (
    <View style={styles.container}>
      <Text style={styles.header}>Quick-SQLite Tester</Text>
      <Button
        title="Refresh"
        onPress={() => {
          const users = queryUsers();
          setUsers(users);
        }}
      />
      <Button
        title="Create user (without transaction)"
        onPress={() => {
          testInsert();
          const users = queryUsers();
          setUsers(users);
        }}
      />
      <Button
        title="Create user (with transaction)"
        onPress={() => {
          testTransaction();
          setTimeout(() => {
            const users = queryUsers();
            setUsers(users);
          }, 1000);
        }}
      />
      <Button
        title="Execute typeORM failing query"
        onPress={executeFailingTypeORMQuery}
      />
      <FlatList
        data={users}
        renderItem={({ item }: ListRenderItemInfo<User>) => {
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
              {!!item.avatar && (
                <Image
                  style={{ width: 64, height: 64 }}
                  source={{
                    uri: `data:image/png;base64,${Buffer.from(
                      item.avatar
                    ).toString('base64')}`,
                  }}
                />
              )}
              <Text style={styles.name}>{item.name}</Text>
              <Text>{item.age}</Text>
              <Text>{item.networth}</Text>
              {/* <Text>{item.metadata.nickname}</Text> */}
              {/* <Text style={{ fontWeight: 'bold', marginTop: 10 }}>
                Favorite Book
              </Text>
              <Text>{info.item.favoriteBook.title}</Text> */}
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
  header: {
    alignSelf: 'center',
    fontWeight: '500',
    fontSize: 20,
  },
  name: {
    fontSize: 20,
  },
});
