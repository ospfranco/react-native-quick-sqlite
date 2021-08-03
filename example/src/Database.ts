import { createConnection, getRepository } from 'typeorm/browser';
import { Book } from './model/Book';
import { User } from './model/User';

export const createDb = async () => {
  await createConnection({
    type: 'react-native',
    database: 'test',
    location: 'default',
    logging: ['error', 'query', 'schema'],
    synchronize: true,
    entities: [User, Book],
  });

  // const bookRepository = getRepository(Book);
  const userRepository = getRepository(User);

  const user1 = new User();
  user1.name = 'John Seedman 🤯';
  user1.age = 30;
  user1.networth = 30000.23;
  user1.metadata = {
    nickname: "<p>We deliver that something because <em>some interesting text!</em></p>\n<p>Always remember...  </p>\n<p><strong>some 🧟‍♀️ 🧚 🍉 text here.</strong></p>\n"
  }

  // const book1 = new Book();
  // book1.title = 'Lord of the rings';
  // book1.user = user1;

  // user1.favoriteBook = book1;

  // await bookRepository.save(book1);
  await userRepository.save(user1);

  const users = await userRepository.find();
  // const users = await userRepository.find({
  //   relations: ['favoriteBook'],
  // });

  return users;
};
