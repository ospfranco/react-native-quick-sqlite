import React, { useEffect, useState } from 'react';
import { SafeAreaView, ScrollView, Text } from 'react-native';
import 'reflect-metadata';
import { registerBaseTests, runTests } from './tests/index';

export default function App() {
  const [results, setResults] = useState([]);

  useEffect(() => {
    setResults([]);
    runTests(registerBaseTests).then(setResults);
  }, []);

  return (
    <SafeAreaView className="flex-1 bg-neutral-900">
      <ScrollView className="p-4">
        <Text className="font-bold text-blue-500 text-lg text-center">
          RN Quick SQLite Test Suite
        </Text>
        {results.map((r, i) => {
          if (r.type === 'grouping') {
            return (
              <Text key={i} className="mt-3 font-bold text-white">
                {r.description}
              </Text>
            );
          }

          if (r.type === 'incorrect') {
            return (
              <Text key={i} className="mt-1 text-white">
                🔴 {r.description}: {r.errorMsg}
              </Text>
            );
          }

          return (
            <Text key={i} className="mt-1 text-white">
              🟢 {r.description}
            </Text>
          );
        })}
      </ScrollView>
    </SafeAreaView>
  );
}
