import { NativeModules } from 'react-native';

type SequelType = {
  multiply(a: number, b: number): Promise<number>;
};

const { Sequel } = NativeModules;

export default Sequel as SequelType;
