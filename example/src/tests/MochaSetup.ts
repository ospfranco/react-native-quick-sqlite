import 'mocha';
import type * as MochaTypes from 'mocha';
// import type { RowItemType } from '../navigators/children/TestingScreen/RowItemType';
import { clearTests, rootSuite } from './MochaRNAdapter';

export async function runTests() {
  // testRegistrators: Array<() => void> = []
  // console.log('setting up mocha');

  const promise = new Promise((resolve) => {
    const {
      EVENT_RUN_BEGIN,
      EVENT_RUN_END,
      EVENT_TEST_FAIL,
      EVENT_TEST_PASS,
      EVENT_SUITE_BEGIN,
      EVENT_SUITE_END,
    } = Mocha.Runner.constants;

    // clearTests();
    const results = [];
    var runner = new Mocha.Runner(rootSuite) as MochaTypes.Runner;

    let indents = -1;
    const indent = () => Array(indents).join('  ');
    runner
      .once(EVENT_RUN_BEGIN, () => {})
      .on(EVENT_SUITE_BEGIN, (suite: MochaTypes.Suite) => {
        const name = suite.fullTitle();
        if (name !== '') {
          results.push({
            indentation: indents,
            description: name,
            key: Math.random().toString(),
            type: 'grouping',
          });
        }
        indents++;
      })
      .on(EVENT_SUITE_END, () => {
        indents--;
      })
      .on(EVENT_TEST_PASS, (test: MochaTypes.Runnable) => {
        results.push({
          indentation: indents,
          description: test.fullTitle(),
          key: Math.random().toString(),
          type: 'correct',
        });
        console.log(`${indent()}pass: ${test.fullTitle()}`);
      })
      .on(EVENT_TEST_FAIL, (test: MochaTypes.Runnable, err: Error) => {
        results.push({
          indentation: indents,
          description: test.fullTitle(),
          key: Math.random().toString(),
          type: 'incorrect',
          errorMsg: err.message,
        });
        console.log(
          `${indent()}fail: ${test.fullTitle()} - error: ${err.message}`
        );
      })
      .once(EVENT_RUN_END, () => {
        resolve(results);
      });

    // testRegistrators.forEach((register) => {
    //   register();
    // });
    runner.run();
  });

  // return () => {
  //   console.log('aborting');
  //   runner.abort();
  // };

  return promise;
}
