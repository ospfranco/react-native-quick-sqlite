import chai from 'chai';
import { describe, it } from './MochaRNAdapter';

export default function () {
  describe('Base tests', () => {
    it('test', async () => {
      chai.assert.fail();
    });
  });
}
