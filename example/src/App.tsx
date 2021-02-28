import * as React from 'react';

import { StyleSheet, View, Text } from 'react-native';
import { initDb } from 'react-native-sequel';

export default function App() {
  const [isDbOpen, setIsDbOpen] = React.useState(false)

  React.useEffect(() => {
    // setIsDbOpen(openDb("sequel.db"))
    initDb()
  }, []);

  return (
    <View style={styles.container}>
      <Text>is db open: {isDbOpen.toString()}</Text>
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
