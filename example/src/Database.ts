import { createConnection, getRepository } from 'typeorm/browser';
import { User } from './model/User';

export const createDb = async () => {
  await createConnection({
    type: 'react-native',
    database: 'test',
    location: 'default',
    logging: ['error', 'query', 'schema'],
    entities: [
      User
    ]
  });

  const user1 = new User();
  user1.name = "Oscar Franco" + new Date().toISOString()

  const userRepository = getRepository(User)
  await userRepository.save(user1)

  console.warn(`user has been saved`)

  const users = await userRepository.find();
  console.warn(`users`, users)
  return users
}