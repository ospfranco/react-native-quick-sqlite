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
  executeFailingTypeORMQuery,
  testAsyncExecute,
  testFailedAsync,
} from './Database';
import type { User } from './model/User';
import { Buffer } from 'buffer';

export default function App() {
  let [users, setUsers] = React.useState<User[]>([]);

  React.useEffect(() => {
    lowLevelInit();
    const users = queryUsers();
    setUsers(users);
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
      <HR />
      <Button
        title="Create user"
        onPress={() => {
          testInsert();
          const users = queryUsers();
          setUsers(users);
        }}
      />
      <Button
        title="Create user async"
        onPress={async () => {
          const users = await testAsyncExecute();
          setUsers(users);
        }}
      />
      <Button
        title="Create user async failure"
        onPress={() => {
          testFailedAsync();
        }}
      />
      <HR />
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
              <Text style={styles.text}>{item.age}</Text>
              <Text style={styles.text}>{item.networth}</Text>
            </View>
          );
        }}
        keyExtractor={(item: any) => item.id}
      />
    </View>
  );
}

function HR() {
  return (
    <View
      style={{
        borderBottomColor: '#333',
        borderBottomWidth: 1,
        marginHorizontal: 10,
        marginVertical: 4,
      }}
    />
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
    marginVertical: 10,
  },
  name: {
    fontSize: 20,
    color: 'black',
  },
  text: {
    color: 'black',
  },
});
