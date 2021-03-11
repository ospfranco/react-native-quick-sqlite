import { createConnection } from 'typeorm/browser';

export const createDb = async () => {
  await createConnection({
    type: 'react-native',
    database: 'test',
    location: 'default',
    logging: ['error', 'query', 'schema'],
    // entities
  })
}