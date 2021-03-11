import {Entity, Column, PrimaryGeneratedColumn } from "typeorm/browser"

@Entity('user')
export class User {
  @PrimaryGeneratedColumn()
  id!: number;

  @Column()
  name!: string;
}