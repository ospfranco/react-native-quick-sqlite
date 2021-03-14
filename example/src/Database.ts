import { createConnection, getRepository } from 'typeorm/browser';
import { User } from './model/User';

export const createDb = async () => {
  await createConnection({
    type: 'react-native',
    database: 'test',
    location: 'default',
    logging: ['error', 'query', 'schema'],
    synchronize: true,
    entities: [
      User
    ]
  });

  // const user1 = new User();
  // user1.name = "Oscar Franco on " + new Date().toISOString();
  // user1.age = 30;
  // user1.networth = 30000.23;

  const userRepository = getRepository(User)
  // await userRepository.save(user1)


  const users = await userRepository.find();
  console.warn(`users`, users)
  return users
  return []
}