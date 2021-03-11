import {Entity, Column, PrimaryGeneratedColumn } from "typeorm/browser"

@Entity('user')
export class User {
  @PrimaryGeneratedColumn("uuid")
  id!: string;

  @Column("text")
  name!: string;
}