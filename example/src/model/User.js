"use strict";
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};
Object.defineProperty(exports, "__esModule", { value: true });
exports.User = void 0;
// import { Book } from './Book'
const browser_1 = require("typeorm/browser");
let User = class User {
    id;
    name;
    age;
    networth;
    // @OneToOne(type => Book)
    // @JoinColumn()
    // favoriteBook!: Book;
    metadata;
    avatar;
};
__decorate([
    (0, browser_1.PrimaryGeneratedColumn)('uuid'),
    __metadata("design:type", String)
], User.prototype, "id", void 0);
__decorate([
    (0, browser_1.Column)('text'),
    __metadata("design:type", String)
], User.prototype, "name", void 0);
__decorate([
    (0, browser_1.Column)('int'),
    __metadata("design:type", Number)
], User.prototype, "age", void 0);
__decorate([
    (0, browser_1.Column)('float'),
    __metadata("design:type", Number)
], User.prototype, "networth", void 0);
__decorate([
    (0, browser_1.Column)('simple-json'),
    __metadata("design:type", Object)
], User.prototype, "metadata", void 0);
__decorate([
    (0, browser_1.Column)('blob'),
    __metadata("design:type", ArrayBuffer)
], User.prototype, "avatar", void 0);
User = __decorate([
    (0, browser_1.Entity)('user')
], User);
exports.User = User;
