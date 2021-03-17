import 'reflect-metadata'
import { User } from './User'
import {Entity, Column, PrimaryGeneratedColumn, OneToOne, JoinColumn } from 'typeorm/browser'

@Entity()
export class Book {
  @PrimaryGeneratedColumn('uuid')
  id!: string;

  @Column()
  title!: string;
  
  @OneToOne(type => User)
  user!: User
}