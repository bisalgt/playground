const fs = require('fs')

// const getNotes = function(fname){
//     fs.readFile(fname, (err, fd) => {
//         if (err) throw err;
//         // console.log(fd);
//         return 'file content';
//     })

// }

const getNotes = function () {
    return 'My Notes...'
}


module.exports = {
    'getNotes': getNotes,
}