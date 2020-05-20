// const fs = require('fs');

// import { readFile } from "fs";

// fs.writeFileSync('notes.txt', 'this file is written again')

// fs.writeFile('notes.txt', 'write to file this', (err) => {
//     console.log(err);
//     console.log('written to file')

// })

// fs.stat('notes.txt', (err, stats) =>{
//     if (err) throw err;
//     console.log(typeof(stats))
//     console.log(JSON.stringify(stats));
// })


// fs.rename('notess.txt', 'notes.txt', (err) => {
//     if (err) throw err;
//     console.log('rename successful');
// })

// fs.appendFile('notes.txt','this is append ot the file', (err) => {
//     if (err) throw err;
//     console.log('appended')
// })

// fs.readFile('notes.txt', (err, datas) => {
//     if (err) throw err;
//     console.log(datas);
// })


// fs.appendFileSync('notes.txt', 'append using append sync');
// console.log('file appended')


// const utils = require('./utils.js')

// // const name = 'Bishal';

// console.log(utils.name);

// utils.add(4,8);


// fs.writeFile('notes.js', '', (err) => {
//     if (err) throw err;
//     console.log('file written')
// })

// const fs = require('fs')


const notesjs = require('./notes.js');

fd = notesjs.getNotes()

console.log(fd)