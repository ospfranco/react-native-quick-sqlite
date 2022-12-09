import {DataSource, Repository} from 'typeorm';
import {beforeAll, beforeEach, it, describe} from './MochaRNAdapter';
import {typeORMDriver} from 'react-native-quick-sqlite';
import {User} from '../model/User';
import {Book} from '../model/Book';
import chai from 'chai';

let expect = chai.expect;

let dataSource: DataSource;
let userRepository: Repository<User>;
let bookRepository: Repository<Book>;

export function registerTypeORMTests() {
  describe('Typeorm tests', () => {
    beforeAll((done: any) => {
      dataSource = new DataSource({
        type: 'react-native',
        database: 'typeormDb.sqlite',
        location: 'default',
        driver: typeORMDriver,
        entities: [User, Book],
        synchronize: true,
      });

      dataSource
        .initialize()
        .then(() => {
          userRepository = dataSource.getRepository(User);
          bookRepository = dataSource.getRepository(Book);
          done();
        })
        .catch(e => {
          console.error('error initializing typeORM datasource', e);
          throw e;
        });
    });

    beforeEach(async () => {
      await userRepository.clear();
      await bookRepository.clear();
    });

    it('basic test', async () => {
      expect(1).to.equal(2);
    });
  });
}
