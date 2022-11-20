import chai from 'chai';
import { beforeEach, describe, it } from './MochaRNAdapter';

export function registerBaseTests() {
  beforeEach(() => {
    console.warn('clearing db');
  });

  describe('Base tests', () => {
    it('test', async () => {
      chai.assert.fail();
    });
  });
}
