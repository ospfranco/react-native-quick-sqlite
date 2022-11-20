import { runTests } from './tests/index';
import 'reflect-metadata';
import React, { useEffect, useState } from 'react';
import {
  StyleSheet,
  Image,
  View,
  Text,
  FlatList,
  ListRenderItemInfo,
  Button,
  SafeAreaView,
} from 'react-native';
// Swap imports to test the typeORM version
import {
  deleteUsers,
  lowLevelInit,
  queryUsers,
  testInsert,
  executeFailingTypeORMQuery,
  testAsyncExecute,
  testAsyncTransactionFailure,
  testAsyncTransactionSuccess,
  testFailedAsync,
  testTransactionFailure,
  testTransactionSuccess,
} from './Database';
import type { User } from './model/User';
import { Buffer } from 'buffer';

export default function App() {
  const [results, setResults] = useState([]);

  useEffect(() => {
    // const tempResults = [];
    runTests().then((r) => {
      console.warn('setting results', r);
      setResults(r);
    });
  }, []);

  return (
    <SafeAreaView style={{ flex: 1 }}>
      <Text>RN Quick SQLite Test Suite</Text>
      {results.map((r, i) => {
        return (
          <Text key={i}>
            {r.description}: {r.type}
          </Text>
        );
      })}
    </SafeAreaView>
  );
  // let [users, setUsers] = React.useState<User[]>([]);
  // React.useEffect(() => {
  //   lowLevelInit();
  //   const users = queryUsers();
  //   setUsers(users);
  // }, []);
  // return (
  //   <View style={styles.container}>
  //     <Text style={styles.header}>Quick-SQLite Tester</Text>
  //     <Button
  //       title="Clear database"
  //       color="red"
  //       onPress={() => {
  //         deleteUsers();
  //         setUsers([]);
  //       }}
  //     />
  //     <Button
  //       title="Refresh"
  //       onPress={() => {
  //         const users = queryUsers();
  //         setUsers(users);
  //       }}
  //     />
  //     <HR />
  //     <Button
  //       title="Create user"
  //       onPress={() => {
  //         testInsert();
  //         const users = queryUsers();
  //         setUsers(users);
  //       }}
  //     />
  //     <Button
  //       title="Create user async"
  //       onPress={async () => {
  //         const users = await testAsyncExecute();
  //         setUsers(users);
  //       }}
  //     />
  //     <Button
  //       title="Create user async failure"
  //       onPress={() => {
  //         testFailedAsync();
  //       }}
  //     />
  //     <HR />
  //     <Text style={styles.sectionHeader}>Transactions</Text>
  //     <Button
  //       title="Create users"
  //       onPress={() => {
  //         testTransactionSuccess();
  //         setTimeout(() => {
  //           const users = queryUsers();
  //           setUsers(users);
  //         }, 1000);
  //       }}
  //     />
  //     <Button
  //       title="Create users failure"
  //       onPress={() => {
  //         testTransactionFailure();
  //         setTimeout(() => {
  //           const users = queryUsers();
  //           setUsers(users);
  //         }, 1000);
  //       }}
  //     />
  //     <Button
  //       title="Create users async"
  //       onPress={() => {
  //         testAsyncTransactionSuccess();
  //         setTimeout(() => {
  //           const users = queryUsers();
  //           setUsers(users);
  //         }, 1000);
  //       }}
  //     />
  //     <Button
  //       title="Create users async failure"
  //       onPress={() => {
  //         testAsyncTransactionFailure();
  //         setTimeout(() => {
  //           const users = queryUsers();
  //           setUsers(users);
  //         }, 1000);
  //       }}
  //     />
  //     <HR />
  //     <Button
  //       title="Execute typeORM failing query"
  //       onPress={executeFailingTypeORMQuery}
  //     />
  //     <FlatList
  //       data={users}
  //       renderItem={({ item }: ListRenderItemInfo<User>) => {
  //         return (
  //           <View
  //             style={{
  //               marginHorizontal: 10,
  //               marginVertical: 4,
  //               borderRadius: 10,
  //               padding: 10,
  //               backgroundColor: 'white',
  //             }}
  //           >
  //             {!!item.avatar && (
  //               <Image
  //                 style={{ width: 64, height: 64 }}
  //                 source={{
  //                   uri: `data:image/png;base64,${Buffer.from(
  //                     item.avatar
  //                   ).toString('base64')}`,
  //                 }}
  //               />
  //             )}
  //             <Text style={styles.name}>{item.name}</Text>
  //             <Text style={styles.text}>{item.age}</Text>
  //             <Text style={styles.text}>{item.networth}</Text>
  //           </View>
  //         );
  //       }}
  //       keyExtractor={(item: any) => item.id}
  //     />
  //   </View>
  // );
}
