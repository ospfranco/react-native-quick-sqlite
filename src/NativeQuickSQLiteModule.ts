import { TurboModuleRegistry, TurboModule } from 'react-native';
  
export interface Spec extends TurboModule {
    install(): boolean;
}

export default TurboModuleRegistry.getEnforcing<Spec>('QuickSQLite');