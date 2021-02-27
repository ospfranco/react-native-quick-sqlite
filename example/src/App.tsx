import * as React from 'react';

import { StyleSheet, View, Text } from 'react-native';
// import { multiplyA } from 'react-native-sequel';

export default function App() {

  React.useEffect(() => {
    openDb("sequel.db")
  }, []);

  return (
    <View style={styles.container}>
      {/* <Text>Result: {result}</Text> */}
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
